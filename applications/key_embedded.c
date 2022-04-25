#include "key_embedded.h"

void key_embedded_init(void)
{
	rt_pin_mode(KEY1, PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(KEY2, PIN_MODE_INPUT_PULLDOWN);
}

rt_bool_t key1_isPressed(void)
{
	if (rt_pin_read(KEY1) == PIN_HIGH) {
		return RT_TRUE;
	} else {
		return RT_FALSE;
	}
}

rt_bool_t key2_isPressed(void)
{
	if (rt_pin_read(KEY2) == PIN_HIGH) {
		return RT_TRUE;
	} else {
		return RT_FALSE;
	}
}