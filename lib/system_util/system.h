#ifndef _system_zym_
#define _system_zym_

#include "esp_wifi.h"
#include "esp_pm.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include <math.h>

#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "inttypes.h"
#include "string.h"
#include "cJSON.h"
#include "driver/uart.h"
#include "driver/gpio.h"


#include "proj_defines.h"

// Functions that other files need to be aware of:

bool setup_initialize();
void setup_succesfull();
void setup_failed();
void declare_state(zym_state state_to_send);
int MQTT_send(char *message);

/*          GENERAL        */
extern int upload_frequency_sec;
extern uint16_t siargo_filter_depth;
extern char access_token[25];

/*       EVENT GROUPS       */
extern EventGroupHandle_t generel_event_group;
extern EventGroupHandle_t wifi_event_group;
extern EventGroupHandle_t mqtt_event_group;


/*      Queues              */
extern QueueHandle_t xQ_state_to_task;
extern QueueHandle_t xQ_state_to_mqtt;
extern QueueHandle_t xQ_send_data;
extern QueueHandle_t xQ_siargo_rpc;
extern QueueHandle_t xQ_alicat_sensor_rpc;

/*          FLOWSENSOR              */
extern char flowsensor_serial[20];
extern char flowsensor_tag[25];

/*          MQTT VARIABLES          */
extern esp_mqtt_client_handle_t client;

/*          OTA VARIABLES           */
extern char firmware_target[30];
extern char current_fw[50];
extern SemaphoreHandle_t xUpdateSemaphore;

/*      Data structs        */
typedef struct
{
    char state[50];
    int64_t time_ms;
} state_struct;

#endif /* _system_zym_ */