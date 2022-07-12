#include "system.h"

// Access token
char access_token[25] = "";


// Semaphores
SemaphoreHandle_t xUpdateSemaphore;

// Queues 
QueueHandle_t xQ_state_to_task = NULL;
QueueHandle_t xQ_state_to_mqtt = NULL;
QueueHandle_t xQ_send_data = NULL;
QueueHandle_t xQ_siargo_rpc = NULL;
QueueHandle_t xQ_alicat_sensor_rpc = NULL;


// Timers
TimerHandle_t SetupTimeout;

// Event groups
EventGroupHandle_t generel_event_group;



// Functions for setup and state
void state_task(void *pvParameters);
static void vSetupTimeout(TimerHandle_t xTimer);



bool setup_initialize()
{
    static const char *TAG = "setup_initialize";

    esp_log_level_set("wifi", ESP_LOG_ERROR);
    esp_log_level_set("wifi_init", ESP_LOG_ERROR);
    esp_log_level_set("smartconfig", ESP_LOG_ERROR);

    /*      GENEREL SETUP           */
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    } 
    ESP_ERROR_CHECK(ret);

    // Init event loop for ESP
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    generel_event_group = xEventGroupCreate();
    
    /*      TIMEOUT TIMER FOR SETUP     */
    SetupTimeout = xTimerCreate("vSetupTimeout", pdMS_TO_TICKS(300000), pdFALSE, (void *)0, vSetupTimeout);
    xTimerStart(SetupTimeout, 0);
    

    /*      STATE TASK SETUP        */
    xQ_state_to_task = xQueueCreate(20, sizeof(zym_state));
    if (xQ_state_to_task == NULL)
    {
        ESP_LOGW(TAG, "FAILED ALLOCATING xQ_state_to_task");
        return false;
    }

    xQ_state_to_mqtt = xQueueCreate(20, sizeof(state_struct));
    if (xQ_state_to_mqtt == NULL)
    {
        ESP_LOGW(TAG, "FAILED ALLOCATING xQ_state_to_mqtt");
        return false;
    }

    if (xTaskCreatePinnedToCore(state_task, "state_task", 4096, NULL, 10, NULL, tskNO_AFFINITY) != pdPASS)
    {
        ESP_LOGW(TAG, "FAILED ALLOCATING state_task");
        return false;
    }

    
    /*      OTA VARIABLE SETUP      */
    xUpdateSemaphore = xSemaphoreCreateMutex();
    if(xUpdateSemaphore == NULL)
    {
        ESP_LOGE(TAG, "FAILED GENERATING OTA SEMAPHORE!");
        return false;
    }

    
    ESP_LOGW(TAG, "RAN SUCCESSFULLY!");
    return true;
}



void setup_succesfull()
{
    ESP_LOGW("ZYMOMETER", "\n\n*********************************\n*\tSETUP COMPLETE!\t\t*\n*********************************\n");
    declare_state(STATE_DEVICE_STARTED);
    xTimerStop(SetupTimeout, 100);
    xTimerDelete(SetupTimeout, 100);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    xEventGroupSetBits(generel_event_group, SETUP_COMPLETE_BIT);
}

void setup_failed()
{
    declare_state(STATE_SETUP_FAILED);
    ESP_LOGE("ZYMOMETER", "SETUP FAILED ..\nRESTARTING ..\nRESTARTING ..\nRESTARTING ..");
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    esp_restart();
}



static void vSetupTimeout(TimerHandle_t xTimer)
{
    //const char *TAG = "vSetupTimeout";
    ESP_LOGE("vSetupTimeout", "SETUP DID NOT FINISH WITHIN 5 MINUTES ..\nRESTARTING ..\nRESTARTING ..\nRESTARTING ..");
    declare_state(STATE_SETUP_FAILED);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    esp_restart();
}


/**
 * @brief Function for sending states the state_task.
 * 
 */
void declare_state(zym_state state_to_send)
{
    zym_state local_state = state_to_send;
    zym_state *plocal_state = &local_state;
    xQueueSend(xQ_state_to_task, plocal_state, (TickType_t)0);
}


/**
 * @brief Receives states, adds timestamp and pushes to queue that sends when MQTT is online.
 *
 */
void state_task(void *pvParameters)
{
    //const char *TAG = "state_task";

    // Data structures
    zym_state Current_state = STATE_UNKNOWN;

    state_struct upload_state = {0};
    state_struct *pupload_state = &upload_state;

    //remembering states so we don't keep sending repeated states
    zym_state last_wifi_state = STATE_UNKNOWN;
    zym_state last_mqtt_state = STATE_UNKNOWN;

    //wait for system time to be set, as this function adds timestamps.
    xEventGroupWaitBits(generel_event_group,
                                        TIME_SET_BIT,
                                        pdFALSE,
                                        pdFALSE,
                                        portMAX_DELAY);

    while (1)
    {
        /* if (xQueueReceive(xQ_state_to_task, &Current_state, (TickType_t)5) == pdPASS)
        {
            pupload_state->time_ms = get_time_ms();
            switch (Current_state)
            {
            case STATE_WIFI_CONNECT:
                last_wifi_state = STATE_WIFI_CONNECT;
                strcpy(pupload_state->state, "STATE_WIFI_CONNECTED");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;

            case STATE_WIFI_DISCONNECT: //We only want to send the initial disconnect, not those from every attempt at connecting.
                if(last_wifi_state == STATE_WIFI_CONNECT)
                {
                    last_wifi_state = STATE_WIFI_DISCONNECT;
                    strcpy(pupload_state->state, "STATE_WIFI_DISCONNECTED");
                    xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                }
                break;

            case STATE_MQTT_CONNECT:
                last_mqtt_state = STATE_MQTT_CONNECT;
                strcpy(pupload_state->state, "STATE_MQTT_CONNECTED");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;

            case STATE_MQTT_DISCONNECT: // Same as STATE_WIFI_DISCONNECT
                if (last_mqtt_state == STATE_MQTT_CONNECT)
                {
                    last_mqtt_state = STATE_MQTT_DISCONNECT;
                    strcpy(pupload_state->state, "STATE_MQTT_DISCONNECT");
                    xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                }
                break;
            case STATE_DEVICE_STARTED:
                strcpy(pupload_state->state, "STATE_DEVICE_STARTED");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_UPLOAD_FREQ_CHANGED:
                strcpy(pupload_state->state, "STATE_UPLOAD_FREQ_CHANGED");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_FILTER_DEPTH_CHANGED:
                strcpy(pupload_state->state, "STATE_FILTER_DEPTH_CHANGED");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_FLOWSENSOR_CONNECTED:
                strcpy(pupload_state->state, "STATE_FLOWSENSOR_CONNECTED");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_FLOWSENSOR_DISCONNECTED:
                strcpy(pupload_state->state, "STATE_FLOWSENSOR_DISCONNECTED");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_NEW_FLOWSENSOR:
                strcpy(pupload_state->state, "STATE_NEW_FLOWSENSOR");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_SETUP_FAILED:
                strcpy(pupload_state->state, "STATE_SETUP_FAILED");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_NEW_FIRMWARE_FOUND:
                strcpy(pupload_state->state, "STATE_NEW_FIRMWARE_FOUND");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_OTA_DOWNLOAD_SUCCESSFUL:
                strcpy(pupload_state->state, "STATE_OTA_DOWNLOAD_SUCCESSFUL");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_OTA_FAILED:
                strcpy(pupload_state->state, "STATE_OTA_FAILED");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_SIARGO_ERROR:
                strcpy(pupload_state->state, "STATE_SIARGO_ERROR");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_PRESSURE_ERROR:
                strcpy(pupload_state->state, "STATE_PRESSURE_ERROR");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_SIARGO_SET_MIN_C02:
                strcpy(pupload_state->state, "STATE_SIARGO_SET_MIN_C02");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_SIARGO_SET_MAX_C02:
                strcpy(pupload_state->state, "STATE_SIARGO_SET_MAX_C02");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_SIARGO_OFFSET_PRESSURE:
                strcpy(pupload_state->state, "STATE_SIARGO_OFFSET_PRESSURE");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_SIARGO_CALIBRATE_FLOWRATE:
                strcpy(pupload_state->state, "STATE_SIARGO_CALIBRATE_FLOWRATE");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_SIARGO_RESET_TOTALIZER:
                strcpy(pupload_state->state, "STATE_SIARGO_RESET_TOTALIZER");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_SIARGO_SET_FILTER_DEPTH:
                strcpy(pupload_state->state, "STATE_SIARGO_SET_FILTER_DEPTH");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_ALICAT_TARE_FLOW:
                strcpy(pupload_state->state, "STATE_ALICAT_TARE_FLOW");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_ALICAT_RESET_TOTALIZER:
                strcpy(pupload_state->state, "STATE_ALICAT_RESET_TOTALIZER");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_ACCESS_TOKEN_WRITTEN:
                strcpy(pupload_state->state, "STATE_ACCESS_TOKEN_WRITTEN");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_WEIGHT_TARE:
                strcpy(pupload_state->state, "STATE_WEIGHT_TARE");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_WEIGHT_ZEROPOINT:
                strcpy(pupload_state->state, "STATE_WEIGHT_ZEROPOINT");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            case STATE_UNKNOWN:
                strcpy(pupload_state->state, "STATE_UNKNOWN");
                xQueueSend(xQ_state_to_mqtt, pupload_state, (TickType_t)0);
                break;
            
            } 
        } */
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}




