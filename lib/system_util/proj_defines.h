#ifndef _project_definitions_zym_
#define _project_definitions_zym_


/********************************************************************
 *                              SETUP                               *
 * *****************************************************************/
// ONLY CHANGE THESE TO DEFINE WHAT YOU WANT TO RUN!

/*          Sensors       */
// #define FLOWSENSOR_ATTACHED
//#define PRESSURESENSOR_ATTACHED
#define SIARGO_ATTACHED
//#define ALICAT_ATTACHED
//#define WEIGHT_ATTACHED
//#define SIMULATE_SENSOR

/*          Wifi          */
#define REDA_WIFI
//#define ZYMOSCOPE_WIFI
//#define FLYING_COUCH_WIFI         // Notice flying couch wifi is 'Flying couch brewing'
//#define EMPIRICAL_WIFI
//#define CUSTOM_WIFI               // Add credentials further down in code

/*       Additional       */
//#define TEST_DEVICE device_1      // Uncomment 'TEST_DEVICE' to broadcast to a device of your choosing.
//#define MEMORY_DEBUG_INFO

// Update this with fx .SIARGO.
#define FW_VERSION "v5.0.2022.06.29.SIARGO_ABS"


/********************************************************************
 *                          REGULAR DEFS                            *
 * *****************************************************************/

#if defined(REDA_WIFI) || defined(ZYMOSCOPE_WIFI) || defined(FLYING_COUCH_WIFI) || defined(CUSTOM_WIFI) || defined(EMPIRICAL_WIFI)
#define HARDCODED_WIFI
#endif


/*          SYSTEM              */
#define UPLOAD_FREQUENCY_SEC 20
#define FW_TITLE "ESP32"


/*          DEVICES             */



/*        Bit definitions       */
#define SETUP_COMPLETE_BIT      BIT0
#define FLOWSENSOR_CHECK_BIT    BIT1
#define TIME_SET_BIT            BIT2

#define MQTT_CONNECTED_BIT      BIT0
#define MQTT_DISCONNECTED_BIT   BIT1
#define MQTT_GET_FW_BIT         BIT2

#define WIFI_CONNECTED_BIT      BIT0
#define WIFI_DISCONNECT_BIT     BIT1
#define ESPTOUCH_DONE_BIT       BIT2
#define WIFI_NVS_FOUND          BIT3


/*      Sensor definitions  */
#define DISABLE_RTS (UART_PIN_NO_CHANGE)
#define DISABLE_CTS (UART_PIN_NO_CHANGE)
#define MAX_SENSOR_AMOUNT 4


/*          SIARGO DEFS         */
#define _SIARGO_FILTER_DEPTH_ 3
#define SIARGO_PRESSURE_TYPE "ABS"

/*          MQTT DEFS           */
#define MQTT_MESSAGESIZE 300


/*          STATE ENUMS         */
typedef enum
{
    STATE_WIFI_CONNECT = -1,
    STATE_WIFI_DISCONNECT = 0,
    STATE_MQTT_CONNECT,
    STATE_MQTT_DISCONNECT,
    STATE_DEVICE_STARTED,
    STATE_UPLOAD_FREQ_CHANGED,
    STATE_FILTER_DEPTH_CHANGED,
    STATE_FLOWSENSOR_CONNECTED,
    STATE_FLOWSENSOR_DISCONNECTED,
    STATE_NEW_FLOWSENSOR,
    STATE_SETUP_FAILED,
    STATE_NEW_FIRMWARE_FOUND,
    STATE_OTA_DOWNLOAD_SUCCESSFUL,
    STATE_OTA_FAILED,
    STATE_SIARGO_ERROR,
    STATE_PRESSURE_ERROR,
    STATE_SIARGO_SET_MIN_C02,
    STATE_SIARGO_SET_MAX_C02,
    STATE_SIARGO_OFFSET_PRESSURE,
    STATE_SIARGO_CALIBRATE_FLOWRATE,
    STATE_SIARGO_RESET_TOTALIZER,
    STATE_SIARGO_SET_FILTER_DEPTH,
    STATE_ALICAT_TARE_FLOW,
    STATE_ALICAT_RESET_TOTALIZER,
    STATE_ACCESS_TOKEN_WRITTEN,
    STATE_WEIGHT_TARE,
    STATE_WEIGHT_ZEROPOINT,
    STATE_UNKNOWN
} zym_state;


/*          RPC ENUMS         */ 
typedef enum
{
    RPC_EMPTY_STATE = -1,
    RPC_SET_MIN_C02 = 0,
    RPC_SET_MAX_C02,
    RPC_SET_OFFSET_GAUGE_PRESSURE,
    RPC_CALIBRATE_FLOWRATE,
    RPC_RESET_ACCUMULATED_FLOW,
    RPC_SET_FILTER_DEPTH
} RPC_siargo;

typedef enum
{
    RPC_ALICAT_EMPTY_STATE = -1,
    RPC_TARE_FLOW = 0,
    RPC_RESET_TOTALIZER
} RPC_alicat;


#endif /* _project_definitions_zym_ */