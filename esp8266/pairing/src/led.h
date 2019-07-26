#ifndef __LED_H__
#define __LED_H__

enum leds {
    LED_GREEN = 0,
    LED_RED,
    LED_MAX
};

enum led_states {
    LED_STATE_OFF = 0,
    LED_STATE_ON,
    LED_STATE_MAX
};

enum led_blink {
    LED_BLINK_OFF = 0,
    LED_BLINK_ON,
    LED_BLINK_MAX
};

void led_init();

int led_start();
int led_stop();

int led_set(int led, int state, int blink);

#endif /* __LED_H__ */