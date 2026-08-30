#include "stm32h7b0xx.h"
#include "toggle_led.h"

/* The LED anode is on PA3, so it is active high. */

void led_on(void)
{
	GPIOA->BSRR = GPIO_BSRR_BS3;
}

void led_off(void)
{
	GPIOA->BSRR = GPIO_BSRR_BR3;
}
