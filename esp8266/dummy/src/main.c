//#include <stdlib.h>
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"

void dummyTask(void *pvParameters)
{
    while(1) {
    }
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    xTaskCreate(dummyTask, "dummyTask", 256, NULL, 2, NULL);
}
