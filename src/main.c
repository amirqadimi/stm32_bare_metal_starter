#include "platform.h"
#include "delay.h"
#include "toggle_led.h"

int main(void)
{
	if (init_platform() != 0)
	{
		return -1;
	}

	for (;;)
	{
		led_on();
		delay_ms(500);
		led_off();
		delay_ms(500);
	}
}
