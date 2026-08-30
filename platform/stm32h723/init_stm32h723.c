/*	Platform bring-up for the STM32H723ZG (RM0468 family), 180 MHz.

	Pins used here. Change them to match your board.

		PE1   user LED, active high
		PA8   MCO1, 16 MHz out
		PA9   SPI2_SCK
		PB14  SPI2_MISO
		PB15  SPI2_MOSI
		PB12  SPI2 CS
		PB10  I2C2_SCL, needs an external pull-up
		PB11  I2C2_SDA, needs an external pull-up
*/

#include "stm32h723xx.h"
#include "delay.h"
#include "toggle_led.h"
#include "spi.h"
#include "i2c.h"

static void mco_init(void)
{
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
	(void)RCC->AHB4ENR;

	GPIOA->MODER &= ~GPIO_MODER_MODER8_Msk;
	GPIOA->MODER |= GPIO_MODER_MODER8_1;

	GPIOA->OTYPER &= ~GPIO_OTYPER_OT8_Msk;

	GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8_Msk;
	GPIOA->OSPEEDR |= (0x3UL << GPIO_OSPEEDR_OSPEED8_Pos);

	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD8_Msk;

	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8_Msk;
}

static void configure_system_clock(void)
{
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);

	/*	VOS1 and 2 wait states carry 180 MHz. Both must be in place before
		SYSCLK moves to the PLL.
	*/
	PWR->D3CR &= ~PWR_D3CR_VOS_Msk;
	PWR->D3CR |= PWR_D3CR_VOS_1;
	while ((PWR->D3CR & PWR_D3CR_VOSRDY) == 0U);

	FLASH->ACR &= ~(FLASH_ACR_LATENCY_Msk | FLASH_ACR_WRHIGHFREQ_Msk);
	FLASH->ACR |= (FLASH_ACR_LATENCY_2WS | FLASH_ACR_WRHIGHFREQ_1);
	while ((FLASH->ACR & (FLASH_ACR_LATENCY_Msk | FLASH_ACR_WRHIGHFREQ_Msk)) !=
		   (FLASH_ACR_LATENCY_2WS | FLASH_ACR_WRHIGHFREQ_1));

	/* MCO1 at 16 MHz. Must be set up before the PLLs. */
	RCC->CFGR &= ~(RCC_CFGR_MCO1_Msk |
				   RCC_CFGR_MCO1PRE_Msk);

	RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
	mco_init();

	/*	PLL1 to 180 MHz off the HSI:

			64 / 32 = 2 MHz, * 90 = 180 MHz VCO, / 1 = 180 MHz
	*/
	RCC->PLLCKSELR &= ~RCC_PLLCKSELR_PLLSRC_Msk;
	RCC->PLLCKSELR |= RCC_PLLCKSELR_PLLSRC_HSI;

	RCC->CR &= ~RCC_CR_PLL1ON;
	while ((RCC->CR & RCC_CR_PLL1RDY) != 0);

	/* HSI divider to 1, so the PLL sees the full 64 MHz. */
	RCC->CR &= ~RCC_CR_HSIDIV;

	/* Integer mode, medium VCO, 2-4 MHz input range. */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL1FRACEN;

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL1VCOSEL;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLL1VCOSEL;

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL1RGE_Msk;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_1;

	RCC->PLLCFGR |= RCC_PLLCFGR_DIVP1EN;

	RCC->PLLCKSELR &= ~RCC_PLLCKSELR_DIVM1_Msk;
	RCC->PLLCKSELR |= (32U << RCC_PLLCKSELR_DIVM1_Pos);

	/* N1 and P1 are stored as (factor - 1). */
	RCC->PLL1DIVR &= ~RCC_PLL1DIVR_N1_Msk;
	RCC->PLL1DIVR |= ((90U - 1U) << RCC_PLL1DIVR_N1_Pos);

	RCC->PLL1DIVR &= ~RCC_PLL1DIVR_P1_Msk;
	RCC->PLL1DIVR |= ((1U - 1U) << RCC_PLL1DIVR_P1_Pos);

	RCC->CR |= RCC_CR_PLL1ON;
	while ((RCC->CR & RCC_CR_PLL1RDY) == 0);

	/* AHB halved to 90 MHz, then every APB halved again. */
	RCC->D1CFGR &= ~RCC_D1CFGR_HPRE_Msk;
	RCC->D1CFGR |= RCC_D1CFGR_HPRE_DIV2;

	RCC->D2CFGR &= ~RCC_D2CFGR_D2PPRE1_Msk;
	RCC->D2CFGR |= RCC_D2CFGR_D2PPRE1_DIV2;

	RCC->D2CFGR &= ~RCC_D2CFGR_D2PPRE2_Msk;
	RCC->D2CFGR |= RCC_D2CFGR_D2PPRE2_DIV2;

	RCC->D3CFGR &= ~RCC_D3CFGR_D3PPRE_Msk;
	RCC->D3CFGR |= RCC_D3CFGR_D3PPRE_DIV2;

	RCC->CFGR &= ~RCC_CFGR_SW_Msk;
	RCC->CFGR |= RCC_CFGR_SW_PLL1;
	while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL1);

	SystemCoreClockUpdate();
}

static void led_gpio_init(void)
{
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
	(void)RCC->AHB4ENR;

	led_off();

	GPIOE->OTYPER &= ~GPIO_OTYPER_OT1_Msk;

	GPIOE->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED1_Msk;

	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD1_Msk;

	GPIOE->MODER &= ~GPIO_MODER_MODER1_Msk;
	GPIOE->MODER |= GPIO_MODER_MODER1_0;
}

void init_platform(void)
{
	configure_system_clock();
	delay_init();
	led_gpio_init();
	spi_init();
	i2c_init();
}
