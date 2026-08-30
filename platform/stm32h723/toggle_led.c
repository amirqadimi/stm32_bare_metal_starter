#include "stm32h723xx.h"
#include "toggle_led.h"

void led_on(void)
{
	GPIOE->BSRR |= GPIO_BSRR_BS1;
}

void led_off(void)
{
	GPIOE->BSRR |= GPIO_BSRR_BR1;
}
