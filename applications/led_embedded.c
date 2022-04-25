#include "led_embedded.h"

void led_embedded_init(void)
{
	rt_pin_mode(LED_EMBEDDED_R, PIN_MODE_OUTPUT);
	rt_pin_mode(LED_EMBEDDED_G, PIN_MODE_OUTPUT);
	rt_pin_mode(LED_EMBEDDED_B, PIN_MODE_OUTPUT);
	led_embedded_off();
};

void led_embedded_blink(rt_int32_t millisecond)
{
	led_embedded_color(LED_RED);
	rt_thread_mdelay(millisecond);
	led_embedded_color(LED_GREEN);
	rt_thread_mdelay(millisecond);
	led_embedded_color(LED_BLUE);
	rt_thread_mdelay(millisecond);
};

void led_embedded_off()
{
	rt_pin_write(LED_EMBEDDED_R, PIN_HIGH);
	rt_pin_write(LED_EMBEDDED_G, PIN_HIGH);
	rt_pin_write(LED_EMBEDDED_B, PIN_HIGH);
}

void led_embedded_color(rt_int8_t color)
{
	switch (color) {
	case LED_WHITE:
		rt_pin_write(LED_EMBEDDED_R, PIN_LOW);
		rt_pin_write(LED_EMBEDDED_G, PIN_LOW);
		rt_pin_write(LED_EMBEDDED_B, PIN_LOW);
		break;

	case LED_RED:
		rt_pin_write(LED_EMBEDDED_R, PIN_LOW);
		rt_pin_write(LED_EMBEDDED_G, PIN_HIGH);
		rt_pin_write(LED_EMBEDDED_B, PIN_HIGH);
		break;

	case LED_GREEN:
		rt_pin_write(LED_EMBEDDED_R, PIN_HIGH);
		rt_pin_write(LED_EMBEDDED_G, PIN_LOW);
		rt_pin_write(LED_EMBEDDED_B, PIN_HIGH);
		break;

	case LED_BLUE:
		rt_pin_write(LED_EMBEDDED_R, PIN_HIGH);
		rt_pin_write(LED_EMBEDDED_G, PIN_HIGH);
		rt_pin_write(LED_EMBEDDED_B, PIN_LOW);
		break;

	default:
		led_embedded_off();
		break;
	}
}
