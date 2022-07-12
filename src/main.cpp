extern "C" {
    void app_main();
    #include "system.h"
}

#include "classes.h"

#ifdef SIARGO_ATTACHED
#include "siargo.h"
#endif



int upload_frequency_sec = UPLOAD_FREQUENCY_SEC;
uint16_t siargo_filter_depth = _SIARGO_FILTER_DEPTH_;
char message[MQTT_MESSAGESIZE];

Sensor *sensors[MAX_SENSOR_AMOUNT];
bool sensors_init();
bool collect_all_sensordata();


void app_main() {
    static const char * TAG = "Main";

    ESP_LOGI(TAG, "Started executing\n\n");
    ESP_LOGW(TAG, "CURRENT FIRMWARE IS: %s, VERSION: %s.", FW_TITLE, FW_VERSION);

/****************************************************
 *                      SETUP                       *
 ****************************************************/

    bool zSetup     = setup_initialize();
    bool zSensor    = sensors_init();   // Initialized second, to set flowsensor_tag for MQTT

    // Checking setup completion
    if (zSetup && zSensor)
    {
        setup_succesfull();
    }
    else{
        if(!zSetup)     {ESP_LOGI(TAG, "Error in: setup");}
        if(!zSensor)    {ESP_LOGI(TAG, "Error in: sensors");}
        setup_failed();
    }


    while(1)
    {
        vTaskDelay(portMAX_DELAY);
    }
}


bool sensors_init()
{
    ESP_LOGI("sensors_init", "");
    ESP_LOGW("sensors_init", "Started executing!");
    int i = 0;

    
    #ifdef SIARGO_ATTACHED
    ESP_LOGW("sensors_init", "SIARGO_ATTACHED");
    sensors[i++] = new Siargosensor();
    #endif


    ESP_LOGW("sensors_init", "Initialized %d sensor", i);
    ESP_LOGW("sensors_init", "Finished executing!");
    return true;
}


bool collect_all_sensordata()
{
    
    return true;
}