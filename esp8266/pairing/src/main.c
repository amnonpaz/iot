#include <espressif/esp_common.h>
#include <esp/uart.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "pairing.h"
#include "led.h"

void user_init(void)
{
    uart_set_baud(0, 115200);

    printf("Device pairing example\n");

    led_init();
    led_start();

    pairingApSetup();

    static struct pairingContext ctx = { {0} };
    xTaskCreate(pairingTask, "pairingTask", 1024, &ctx, 2, NULL);
}
