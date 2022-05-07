#ifndef MY_LED_EMBEDDED_H
#define MY_LED_EMBEDDED_H

#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>

#define LED_WHITE 0b111
#define LED_RED 0b100
#define LED_GREEN 0b010
#define LED_BLUE 0b001

void led_embedded_init(void);
void led_embedded_blink(rt_int32_t millisecond);
void led_embedded_off();
void led_embedded_color(rt_int8_t color);

#endif
