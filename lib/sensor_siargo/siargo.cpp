#include "siargo.h"


void siargo_read_task(void *pvParameters);
void siargo_dataprocess_task(void *pvParameters);

char flowrate_avg[20];
char accumulated_flowrate_s[20];
char temperature_avg[20];
char humidity_avg[20];
char pressure_avg[20];
char concentration_avg[20];


Siargosensor::Siargosensor()
{
    TAG = "Siargo";

    this_object = this;
    test_pressure = 0;

    setup();

    ESP_LOGW(TAG, "Finished setup!");
}


bool Siargosensor::setup()
{

    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {.clk_speed = I2C_MASTER_FREQ_HZ},
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL,
    //.master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

    UART_data = NULL;

    I2C_data = (uint8_t *)malloc(I2C_RECEIVE_LENGTH);

    writeprotect_disabled = false;

    xGetSetSemaphore = xSemaphoreCreateMutex();
    if (xGetSetSemaphore == NULL)
    {
        ESP_LOGW(TAG, "FAILED CREATING MUTEX!");
        return false;
    }

    xQ_siargo_rpc = xQueueCreate(10, sizeof(RPC_siargo));
    if (xQ_siargo_rpc == NULL)
    {
        ESP_LOGW(TAG, "FAILED ALLOCATING RPC QUEUE");
        return false;
    }

    if (xTaskCreatePinnedToCore(siargo_read_task, "siargo_read_task", 2048, this_object, 10, &xReadtask_handle, tskNO_AFFINITY) != pdPASS)
    {
        ESP_LOGW(TAG, "FAILED ALLOCATING READ TASK");
        return false;
    }

    return true;
}



/********************************************************************
 *                    Read, parse and collect data                  *
 * *****************************************************************/

void siargo_read_task(void *pvParameters)
{
    // Reference to object that started task
    Siargosensor *calling_object = (Siargosensor *)pvParameters;

    // I2C address
    uint8_t addr[2] = {(uint8_t)0x00, (uint8_t)0x45};

    // wait for setup completion
    xEventGroupWaitBits(generel_event_group,
                        SETUP_COMPLETE_BIT,
                        pdFALSE,
                        pdFALSE,
                        portMAX_DELAY);

    while (1)
    {
        memset(calling_object->I2C_data, 0xFF, I2C_RECEIVE_LENGTH);
        if (read_register(I2C_MASTER_NUM, 0x01, addr, 2, calling_object->I2C_data, 6) == ESP_OK)
        {
            calling_object->test_data_parser();

        }
        vTaskDelay(15 / portTICK_RATE_MS);
    }
}


bool Siargosensor::data_parser(siargo_struct *pData)
{
    return true;
}


bool Siargosensor::test_data_parser()
{

    uint8_t *I2Cpointer = I2C_data;

    uint8_t pressure[6];
    memcpy(pressure, I2Cpointer, 6);
    if (validate_crc(pressure, sizeof(pressure)) == ESP_FAIL)
    {
        printf("Error in crc of siargo pressure!\n");
        return false;
    }
    test_pressure = (float)to_uint32(pressure) / 100;
    printf("Byte0: %" PRIu8 "   Byte1: %" PRIu8 "   Byte3: %" PRIu8 "   Byte4: %" PRIu8 "\t Pressure: %f \n\n", pressure[0], pressure[1], pressure[3], pressure[4], test_pressure);
    return true;
}


void Siargosensor::collect_data(cJSON *values)
{

}



/********************************************************************
 *                   GET, SET AND RESET HELPER                      *
 * *****************************************************************/

void Siargosensor::data_set_helper(void *pData)
{

}


bool Siargosensor::data_get_helper(void *pData) 
{
    return true;
}


void Siargosensor::data_reset_helper() 
{
}



