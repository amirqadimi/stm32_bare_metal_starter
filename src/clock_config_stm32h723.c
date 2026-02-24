
#include "stm32h723xx.h"

void SystemClockConfig(void)
{
	// 1. Enable HSI
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);

	// 2. Configure voltage scaling and Flash latency for 180 MHz operation
	PWR->D3CR &= ~PWR_D3CR_VOS_Msk;
	PWR->D3CR |= PWR_D3CR_VOS_1;
	while ((PWR->D3CR & PWR_D3CR_VOSRDY) == 0U);

	FLASH->ACR &= ~(FLASH_ACR_LATENCY_Msk | FLASH_ACR_WRHIGHFREQ_Msk);
	FLASH->ACR |= (FLASH_ACR_LATENCY_2WS | FLASH_ACR_WRHIGHFREQ_1);
	while ((FLASH->ACR & (FLASH_ACR_LATENCY_Msk | FLASH_ACR_WRHIGHFREQ_Msk)) !=
	       (FLASH_ACR_LATENCY_2WS | FLASH_ACR_WRHIGHFREQ_1));

	// 3. Select HSI as PLL source
	RCC->PLLCKSELR &= ~RCC_PLLCKSELR_PLLSRC_Msk;
	RCC->PLLCKSELR |= RCC_PLLCKSELR_PLLSRC_HSI;
	
	/*	We target 180Mhz as the output of the PLL1
		F(vco) = (F(in)/m)*N 
		F(out) = F(vco)/P 
		F(out) = ((64/32)*90)/1 = 180MHz
		M = 32, N = 90 and P = 1
	*/ 
	
	// 4. Config PLL1 

	// Disable PLL1 before configuration
	RCC->CR &= ~RCC_CR_PLL1ON;
	while ((RCC->CR & RCC_CR_PLL1RDY) != 0); 

	// Set HSI divider to 1
	RCC->CR &= ~RCC_CR_HSIDIV;

	// Disable fractional calculation
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL1FRACEN;

	// Select medium range for VCO
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL1VCOSEL;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLL1VCOSEL;

	// Select input frequency range
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL1RGE_Msk;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_1;

	// Enable divider
	RCC->PLLCFGR |= RCC_PLLCFGR_DIVP1EN;

	// Clear DIVM1 and write the raw divider factor
	RCC->PLLCKSELR &= ~RCC_PLLCKSELR_DIVM1_Msk;
	RCC->PLLCKSELR |= (32U << RCC_PLLCKSELR_DIVM1_Pos);

	// Program PLL multipliers/dividers (register stores factor - 1)
	RCC->PLL1DIVR &= ~RCC_PLL1DIVR_N1_Msk; 
	RCC->PLL1DIVR |= ((90U - 1U) << RCC_PLL1DIVR_N1_Pos); 

	RCC->PLL1DIVR &= ~RCC_PLL1DIVR_P1_Msk;
	RCC->PLL1DIVR |= ((1U - 1U) << RCC_PLL1DIVR_P1_Pos);

	// 5. Enable PLL1
	RCC->CR |= RCC_CR_PLL1ON;
	while ((RCC->CR & RCC_CR_PLL1RDY) == 0); 

	// 6. Set AHB, APB1, APB2 prescalers (to /2 for safety)
	RCC->D1CFGR = (RCC->D1CFGR & ~RCC_D1CFGR_HPRE_Msk) | RCC_D1CFGR_HPRE_3;
	RCC->D2CFGR = (RCC->D2CFGR & ~RCC_D2CFGR_D2PPRE1_Msk) | RCC_D2CFGR_D2PPRE1_2;
	RCC->D2CFGR = (RCC->D2CFGR & ~RCC_D2CFGR_D2PPRE2_Msk) | RCC_D2CFGR_D2PPRE2_2;

	// 7. Switch SYSCLK to PLL1
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;
	RCC->CFGR |= RCC_CFGR_SW_PLL1;
	while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL1);

	// 8. Update SystemCoreClock variable
	SystemCoreClockUpdate();
}
