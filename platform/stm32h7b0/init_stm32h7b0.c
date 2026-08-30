/*	Platform bring-up for the STM32H7B0VBT6 (RM0455 family), 280 MHz.

	Pins used here. Change them to match your board.

		PA3   user LED, active high
		PA8   MCO1, 16 MHz out
		PA9   SPI2_SCK
		PC1   SPI2_MOSI
		PC2_C SPI2_MISO
		PB12  SPI2 CS
		PB10  I2C2_SCL, needs an external pull-up
		PB11  I2C2_SDA, needs an external pull-up
		PH0   HSE in, 25 MHz crystal
		PH1   HSE out

	The MCU runs from its internal LDO, hence the USE_PWR_LDO_SUPPLY define.
*/

#include "stm32h7b0xx.h"
#include "delay.h"
#include "toggle_led.h"
#include "spi.h"
#include "i2c.h"

/* Spin count before giving up on the crystal and falling back to the HSI. */
#define HSE_STARTUP_TIMEOUT 2000000U

/* VOS0 is the only scale that allows 280 MHz on the STM32H7A3/B3/B0 line. */
static void configure_voltage_scaling(void)
{
	PWR->SRDCR |= PWR_SRDCR_VOS_Msk;

	while ((PWR->SRDCR & PWR_SRDCR_VOSRDY) == 0U);
	while ((PWR->CSR1 & PWR_CSR1_ACTVOSRDY) == 0U);
}

/*	Flash at 280 MHz needs 6 wait states (RM0455 Table 16), set before SYSCLK
	moves to the PLL.

	Write ACR in one shot. A read-modify-write would clear LATENCY to 0 first,
	and this silicon cannot run at 0 wait states even on the HSI: the prefetch
	reads ECC-corrupt data during that window and takes a bus fault.
*/
static void configure_flash_latency(void)
{
	FLASH->ACR = FLASH_ACR_LATENCY_6WS | FLASH_ACR_WRHIGHFREQ;

	while ((FLASH->ACR & FLASH_ACR_LATENCY_Msk) != FLASH_ACR_LATENCY_6WS);
}

/* Returns 1 when the crystal came up, 0 otherwise. */
static int enable_hse(void)
{
	uint32_t timeout = HSE_STARTUP_TIMEOUT;

	RCC->CR &= ~RCC_CR_HSEBYP;
	RCC->CR |= RCC_CR_HSEON;

	while ((RCC->CR & RCC_CR_HSERDY) == 0U)
	{
		if (--timeout == 0U)
		{
			RCC->CR &= ~RCC_CR_HSEON;
			return 0;
		}
	}

	return 1;
}

/*	PLL1 to 280 MHz, either source landing on the same 560 MHz VCO:

		HSE:  25 / 5  = 5 MHz, * 112 = 560, / 2 = 280
		HSI:  64 / 16 = 4 MHz, * 140 = 560, / 2 = 280
*/
static void configure_pll1(int use_hse)
{
	const uint32_t divm = use_hse ? 5U : 16U;
	const uint32_t divn = use_hse ? 112U : 140U;
	const uint32_t divp = 2U;

	RCC->CR &= ~RCC_CR_PLL1ON;
	while ((RCC->CR & RCC_CR_PLL1RDY) != 0U);

	RCC->PLLCKSELR &= ~RCC_PLLCKSELR_PLLSRC_Msk;
	RCC->PLLCKSELR |= use_hse ? RCC_PLLCKSELR_PLLSRC_HSE
							  : RCC_PLLCKSELR_PLLSRC_HSI;

	RCC->PLLCKSELR &= ~RCC_PLLCKSELR_DIVM1_Msk;
	RCC->PLLCKSELR |= (divm << RCC_PLLCKSELR_DIVM1_Pos);

	/* Integer mode, wide VCO, 4-8 MHz input range. */
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLL1FRACEN |
					  RCC_PLLCFGR_PLL1VCOSEL |
					  RCC_PLLCFGR_PLL1RGE_Msk);
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLL1RGE_1 | RCC_PLLCFGR_DIVP1EN);

	/* The dividers are stored as (factor - 1). */
	RCC->PLL1DIVR &= ~(RCC_PLL1DIVR_N1_Msk | RCC_PLL1DIVR_P1_Msk);
	RCC->PLL1DIVR |= ((divn - 1U) << RCC_PLL1DIVR_N1_Pos) |
					 ((divp - 1U) << RCC_PLL1DIVR_P1_Pos);

	RCC->CR |= RCC_CR_PLL1ON;
	while ((RCC->CR & RCC_CR_PLL1RDY) == 0U);
}

/* CPU and AHB at 280 MHz, every APB halved to stay under the 140 MHz limit. */
static void configure_bus_prescalers(void)
{
	RCC->CDCFGR1 &= ~(RCC_CDCFGR1_CDCPRE_Msk |
					  RCC_CDCFGR1_HPRE_Msk |
					  RCC_CDCFGR1_CDPPRE_Msk);
	RCC->CDCFGR1 |= (RCC_CDCFGR1_CDCPRE_DIV1 |
					 RCC_CDCFGR1_HPRE_DIV1 |
					 RCC_CDCFGR1_CDPPRE_DIV2);

	RCC->CDCFGR2 &= ~(RCC_CDCFGR2_CDPPRE1_Msk | RCC_CDCFGR2_CDPPRE2_Msk);
	RCC->CDCFGR2 |= (RCC_CDCFGR2_CDPPRE1_DIV2 | RCC_CDCFGR2_CDPPRE2_DIV2);

	RCC->SRDCFGR &= ~RCC_SRDCFGR_SRDPPRE_Msk;
	RCC->SRDCFGR |= RCC_SRDCFGR_SRDPPRE_DIV2;
}

static void configure_system_clock(void)
{
	int use_hse;

	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0U);

	configure_voltage_scaling();
	configure_flash_latency();

	use_hse = enable_hse();

	configure_pll1(use_hse);
	configure_bus_prescalers();

	RCC->CFGR &= ~RCC_CFGR_SW_Msk;
	RCC->CFGR |= RCC_CFGR_SW_PLL1;
	while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL1);

	SystemCoreClockUpdate();
}

/*	16 MHz out of PA8, sourced from the HSI so it stays steady whether or not
	the PLL runs off the crystal. AF0 is MCO1, so the AFR nibble stays zero.
*/
static void mco1_init(void)
{
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
	(void)RCC->AHB4ENR;

	/* MCO1 source = HSI, prescaler = /4. */
	RCC->CFGR &= ~(RCC_CFGR_MCO1_Msk | RCC_CFGR_MCO1PRE_Msk);
	RCC->CFGR |= RCC_CFGR_MCO1PRE_2;

	GPIOA->OTYPER &= ~GPIO_OTYPER_OT8_Msk;

	GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8_Msk;
	GPIOA->OSPEEDR |= (0x3UL << GPIO_OSPEEDR_OSPEED8_Pos);

	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD8_Msk;

	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8_Msk;

	GPIOA->MODER &= ~GPIO_MODER_MODER8_Msk;
	GPIOA->MODER |= GPIO_MODER_MODER8_1;
}

static void led_gpio_init(void)
{
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
	(void)RCC->AHB4ENR;

	led_off();

	GPIOA->OTYPER &= ~GPIO_OTYPER_OT3_Msk;

	GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED3_Msk;

	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD3_Msk;

	GPIOA->MODER &= ~GPIO_MODER_MODER3_Msk;
	GPIOA->MODER |= GPIO_MODER_MODER3_0;
}

void init_platform(void)
{
	configure_system_clock();
	delay_init();
	led_gpio_init();
	mco1_init();
	spi_init();
	i2c_init();
}
