#ifndef _UTIL_I2C_
#define _UTIL_I2C_

extern "C" {
    #include "system.h"
}
#include "driver/i2c.h"

#define I2C_MASTER_NUM 0   

esp_err_t read_register(i2c_port_t i2c_port, uint8_t bus_address, uint8_t *register_addr, size_t register_addr_size, uint8_t *data_rd, size_t size);
esp_err_t write_register(i2c_port_t i2c_port, uint8_t bus_address, uint8_t *register_addr, size_t register_addr_size, uint8_t *data_rd, size_t size);

uint16_t to_uint16(uint8_t *data);
uint32_t to_uint32(uint8_t *data);

esp_err_t validate_crc(uint8_t *data_rd, size_t size);
esp_err_t fill_crc(uint8_t *data_rd, size_t size);
uint8_t crc8(uint8_t *data, uint8_t len);


#endif