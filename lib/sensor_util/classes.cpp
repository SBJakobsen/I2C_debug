#include "classes.h"


void Sensor::data_set(void *pData)
{
    if (xGetSetSemaphore == NULL)
    {
        ESP_LOGI(TAG, "Semaphore is null");
        return;
    }

    if (xSemaphoreTake(xGetSetSemaphore, (TickType_t)20) == pdTRUE)
    {
        data_set_helper(pData);
        xSemaphoreGive(xGetSetSemaphore);
    }
    else
    {
        ESP_LOGI(TAG, "Couldn't get semaphore in time");
    }
    return;
}

bool Sensor::data_get(void *pData)
{

    // Check that semaphore exists
    if (xGetSetSemaphore == NULL)
    {
        ESP_LOGI(TAG, "Semaphore is null");
        return false;
    }

    // Attempt to take semaphore
    if (xSemaphoreTake(xGetSetSemaphore, (TickType_t)20) == pdTRUE)
    {
        if (!data_get_helper(pData)) 
        {
            xSemaphoreGive(xGetSetSemaphore);
            return false; 
        }
        // We're done with the semaphore
        xSemaphoreGive(xGetSetSemaphore);
    }
    else
    {
        ESP_LOGI(TAG, "Couldn't get semaphore in time");
        return false;
    }
    return true;
}

bool Sensor::data_reset()
{
    // Check that semaphore exists
    if (xGetSetSemaphore == NULL)
    {
        ESP_LOGI(TAG, "Semaphore is null");
        return false;
    }

    // Attempt to take semaphore
    if (xSemaphoreTake(xGetSetSemaphore, (TickType_t)20) == pdTRUE)
    {
        data_reset_helper();
        // We're done with the semaphore
        xSemaphoreGive(xGetSetSemaphore);
    }
    else
    {
        ESP_LOGI(TAG, "Couldn't get semaphore in time");
        return false;
    }
    return true;
}  

