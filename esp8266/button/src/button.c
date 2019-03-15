#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "esp8266.h"

const int gpio_button = 5;
const int gpio_led = 4;
volatile int led_state = 0;

void gpio_intr_handler(uint8_t gpio_num);

void buttonIntTask(void *pvParameters)
{
    QueueHandle_t *tsqueue = (QueueHandle_t *)pvParameters;
    gpio_set_interrupt(gpio_button, GPIO_INTTYPE_EDGE_NEG, gpio_intr_handler);

    uint32_t last = 0;
    while (1) {
        uint32_t button_ts;
        xQueueReceive(*tsqueue, &button_ts, portMAX_DELAY);
        button_ts *= portTICK_PERIOD_MS;
        if (last < button_ts - 200) {
            last = button_ts;

	    led_state = 1 - led_state;
	    gpio_write(gpio_led, led_state);
        }
    }
}

static QueueHandle_t tsqueue;

void gpio_intr_handler(uint8_t gpio_num)
{
    uint32_t now = xTaskGetTickCountFromISR();
    xQueueSendToBackFromISR(tsqueue, &now, NULL);
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    gpio_enable(gpio_button, GPIO_INPUT);
    gpio_enable(gpio_led, GPIO_OUTPUT);
    gpio_write(gpio_led, led_state);

    tsqueue = xQueueCreate(2, sizeof(uint32_t));
    xTaskCreate(buttonIntTask, "buttonIntTask", 256, &tsqueue, 2, NULL);
}
