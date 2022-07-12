#include "util_i2c.h"


const char *TAG_CRC = "Siargo_crc";



/********************************************************************
 *                  Read and write a register                       *
 * *****************************************************************/

esp_err_t read_register(i2c_port_t i2c_port, uint8_t bus_address, uint8_t *register_addr, size_t register_addr_size, uint8_t *data_rd, size_t size)
{

    esp_err_t ret = ESP_FAIL;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_start(cmd));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, (bus_address << 1) | I2C_MASTER_WRITE, true));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write(cmd, register_addr, register_addr_size, true));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_start(cmd));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, (bus_address << 1) | I2C_MASTER_READ, true));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_read(cmd, data_rd, size, I2C_MASTER_LAST_NACK));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_stop(cmd));
    ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t write_register(i2c_port_t i2c_port, uint8_t bus_address, uint8_t *register_addr, size_t register_addr_size, uint8_t *data_rd, size_t size)
{

    esp_err_t ret = ESP_FAIL;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_start(cmd));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, (bus_address << 1) | I2C_MASTER_WRITE, true));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write(cmd, register_addr, register_addr_size, true));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write(cmd, data_rd, size, true));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_stop(cmd));
    ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}



/********************************************************************
 *                      CRC RELATED FUNCTIONS                       *
 * *****************************************************************/

esp_err_t validate_crc(uint8_t *data_rd, size_t size)
{
    
    //ESP_LOGV(TAG_CRC, "Validate CRC of Bytes:");
    //ESP_LOG_BUFFER_HEX_LEVEL(TAG_CRC, data_rd, size, ESP_LOG_VERBOSE);

    for (size_t i = 2; i < size; i += 3)
    {
        uint8_t crc[2] = {data_rd[i - 2], data_rd[i - 1]};
        uint8_t res = crc8(crc, 2);

        if (data_rd[i] == res)
        {
            ESP_LOGV(TAG_CRC, "CRC8 Valid Expected:%02X Result:%02X ", res, data_rd[i]);
        }
        else
        {
            ESP_LOGE(TAG_CRC, "CRC8 of chunk index[%i]{ %02X, %02X } did not match.\n(Expected:%02X Result:%02X)\nAll input bytes:", i, crc[0], crc[1], data_rd[i], res);
            ESP_LOG_BUFFER_HEX_LEVEL(TAG_CRC, data_rd, size, ESP_LOG_ERROR);
            return ESP_FAIL;
        }
    }

    ESP_LOGV(TAG_CRC, "Validation passed!");

    return ESP_OK;
}


esp_err_t fill_crc(uint8_t *data_rd, size_t size)
{
    ESP_LOGV(TAG_CRC, "Fill CRC of Bytes:");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG_CRC, data_rd, size, ESP_LOG_VERBOSE);

    for (size_t i = 2; i < size; i += 3)
    {
        uint8_t crc[2] = {data_rd[i - 2], data_rd[i - 1]};
        data_rd[i] = crc8(crc, 2);
    }

    ESP_LOGV(TAG_CRC, "Calculated all CRC bytes.");

    return ESP_OK;
}


uint8_t crc8(uint8_t *data, uint8_t len)
{
    uint8_t crc = 0x00; // Init with 0x00

    for (uint8_t x = 0; x < len; x++)
    {
        crc ^= data[x]; // XOR-in the next input byte

        for (uint8_t i = 0; i < 8; i++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x07);
            else
                crc <<= 1;
        }
    }

    return crc;
}


/********************************************************************
 *                      CONVERSIONS OF BYTE DATA                    *
 * *****************************************************************/

uint16_t to_uint16(uint8_t *data)
{
    return data[0] << 8 | data[1];
}

uint32_t to_uint32(uint8_t *data)
{
    return data[0] << 24 | data[1] << 16 | data[3] << 8 | data[4];
}