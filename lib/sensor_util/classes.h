#ifndef _CLASS_ZYM_
#define _CLASS_ZYM_

extern "C"
{
    #include "system.h"
}

class Sensor
{
    protected:
        SemaphoreHandle_t xGetSetSemaphore;
        TaskHandle_t xReadtask_handle;
        void *this_object;
        

    public:
        uint8_t *UART_data;
        const char *TAG;
        
        void data_set(void *pData);
        bool data_get(void *pData);
        bool data_reset();
        virtual void data_set_helper(void *pData) = 0;
        virtual bool data_get_helper(void *pData) = 0;
        virtual void data_reset_helper() = 0;
        virtual void collect_data(cJSON *values) = 0;
};


#endif