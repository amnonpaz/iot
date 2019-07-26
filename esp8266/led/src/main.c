#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"

#include "led.h"

void light_change_task(void *pvParameters)
{
    int state = 0;
    int blink = 0;

    while(1) {
        led_set(LED_GREEN, state,  blink);
        led_set(LED_RED,   !state, !blink);
        vTaskDelay(2200 / portTICK_PERIOD_MS);

        if (blink)
            state = !state;
        blink = !blink;
    }
}

void user_init(void)
{
    uart_set_baud(0, 115200);

    led_init();
    led_start();

    xTaskCreate(light_change_task, "light_change_task", 256, NULL, 2, NULL);
}
