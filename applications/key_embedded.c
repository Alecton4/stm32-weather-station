#include "key_embedded.h"

// not really useful, just for initing interrupt
volatile rt_bool_t key1_isChanged = RT_FALSE;
volatile rt_bool_t key2_isChanged = RT_FALSE;

static void key1_change(void);
static void key2_change(void);

void key_embedded_init(void)
{
	rt_pin_mode(KEY1_EMBEDDED, PIN_MODE_INPUT_PULLDOWN);
	rt_pin_attach_irq(KEY1_EMBEDDED, PIN_IRQ_MODE_RISING, key1_change, RT_NULL);
	rt_pin_irq_enable(KEY1_EMBEDDED, PIN_IRQ_ENABLE);
	rt_pin_mode(KEY2_EMBEDDED, PIN_MODE_INPUT_PULLDOWN);
	rt_pin_attach_irq(KEY2_EMBEDDED, PIN_IRQ_MODE_RISING, key2_change, RT_NULL);
	rt_pin_irq_enable(KEY2_EMBEDDED, PIN_IRQ_ENABLE);
}

rt_bool_t key1_isPressed(void)
{
	if (rt_pin_read(KEY1_EMBEDDED) == PIN_HIGH) {
		return RT_TRUE;
	} else {
		return RT_FALSE;
	}
}

rt_bool_t key2_isPressed(void)
{
	if (rt_pin_read(KEY2_EMBEDDED) == PIN_HIGH) {
		return RT_TRUE;
	} else {
		return RT_FALSE;
	}
}

static void key1_change(void)
{
	key1_isChanged = RT_TRUE;
}

static void key2_change(void)
{
	key2_isChanged = RT_TRUE;
}
