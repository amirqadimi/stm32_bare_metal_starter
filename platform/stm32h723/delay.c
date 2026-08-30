#include "delay.h"
#include "stm32h723xx.h"

static volatile uint32_t s_ticks;

void SysTick_Handler(void)
{
	s_ticks++;
}

void delay_init(void)
{
	SysTick_Config(SystemCoreClock / 1000U);
}

uint32_t get_millis(void)
{
	return s_ticks;
}

void delay_ms(uint32_t ms)
{
	uint32_t start = s_ticks;

	while ((s_ticks - start) < ms)
	{
		__WFI();
	}
}