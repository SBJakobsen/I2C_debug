#ifndef _SIARGO_
#define _SIARGO_


#include "classes.h"
#include "util_i2c.h"


#define I2C_MASTER_SCL_IO 22 /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 21 /*!< GPIO number used for I2C master data  */

#define I2C_MASTER_NUM 0            /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

#define I2C_RECEIVE_LENGTH 42

#define FILTER_DEPTH_SIARGO 3

struct siargo_struct
{
    float flowrate;
    float accumulated_flowrate;
    float temperature;
    float humidity;
    float pressure;
    float concentration;
    int measurements;
    siargo_struct()
    {
        flowrate = 0;
        accumulated_flowrate = 0; 
        temperature = 0;
        humidity = 0;
        pressure = 0;
        concentration = 0;
        measurements = 0;
    }
};

class Siargosensor : public Sensor
{
    private:
        siargo_struct readings;    
        float test_pressure;
    public:
        uint8_t *I2C_data;
        bool writeprotect_disabled;
        Siargosensor();
        void data_set_helper(void *pData) override;
        bool data_get_helper(void *pData) override;
        void data_reset_helper() override;
        void collect_data(cJSON *values);

        bool setup();
        bool data_parser(siargo_struct *pData);
        bool test_data_parser();
};


#endif /* _SIARGO_FS36210_I2C_ */