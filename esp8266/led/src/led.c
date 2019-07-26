#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"

#define BLINK_PERIOD_MS 250

static int leds_pins_map[LED_MAX] = { 5, 4 };

static struct {
    TaskHandle_t task_handle;
    int init;
    int suspended;

    struct {
        int state;
        int blink;
    } leds[LED_MAX];
} led_ctx = { 0 };

static void led_task(void *pvParameters)
{
    while (1) {
        for (int i = 0; i < LED_MAX; i++)
            gpio_write(leds_pins_map[i], led_ctx.leds[i].state);

        vTaskDelay(BLINK_PERIOD_MS / portTICK_PERIOD_MS);

        for (int i = 0; i < LED_MAX; i++)
            gpio_write(leds_pins_map[i], led_ctx.leds[i].state && !led_ctx.leds[i].blink);

        vTaskDelay(BLINK_PERIOD_MS / portTICK_PERIOD_MS);
    }
}

void led_init()
{
    if (led_ctx.init)
        return;

    xTaskCreate(led_task, "led_task", 256, &led_ctx, 2, &led_ctx.task_handle);

    vTaskSuspend(led_ctx.task_handle);
    led_ctx.suspended = 1;
    led_ctx.init = 1;
}

int led_start()
{
    if (!led_ctx.suspended || !led_ctx.init)
        return 0;

    for (int i = 0; i < LED_MAX; i++)
        gpio_enable(leds_pins_map[i], GPIO_OUTPUT);

    vTaskResume(led_ctx.task_handle);
    led_ctx.suspended = 0;

    return 1;
}

int led_stop()
{
    if (led_ctx.suspended || !led_ctx.init)
        return 0;

    vTaskSuspend(led_ctx.task_handle);
    led_ctx.suspended = 1;

    for (int i = 0; i < LED_MAX; i++)
        gpio_disable(leds_pins_map[i]);

    return 1;
}

int led_set(int led, int state, int blink)
{
    if (led >= LED_MAX || state >= LED_STATE_MAX || blink >= LED_BLINK_MAX)
        return 0;

    led_ctx.leds[led].state = state;
    led_ctx.leds[led].blink = blink;

    return 1;
}