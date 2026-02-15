
#include "stm32h723xx.h"

// Simple clock config: HSI (64MHz) -> PLL1 -> SYSCLK (200MHz)
// Safe bus prescalers, enable UART/I2C clocks
void SystemClockConfig(void)
{
	// 1. Enable HSI (should be on by default, but ensure it)
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);

	// 2. Select HSI as PLL1 source
	RCC->PLLCKSELR &= ~RCC_PLLCKSELR_PLLSRC;
	RCC->PLLCKSELR |= (0x3 << RCC_PLLCKSELR_PLLSRC_Pos); // 0x3 = HSI

	// 3. Configure PLL1: VCO = (HSI/PLLM) * PLLN
	// Target: 200MHz SYSCLK
	// HSI = 64MHz, PLLM = 8, PLLN = 50, PLLP = 2
	// VCO = 64/8*50 = 400MHz, PLLP = 2, SYSCLK = 400/2 = 200MHz
	RCC->PLLCKSELR = (RCC->PLLCKSELR & ~(RCC_PLLCKSELR_DIVM1_Msk)) | (8 << RCC_PLLCKSELR_DIVM1_Pos);
	RCC->PLL1DIVR = ((50-1) << RCC_PLL1DIVR_N1_Pos) | ((2-1) << RCC_PLL1DIVR_P1_Pos) | ((2-1) << RCC_PLL1DIVR_Q1_Pos) | ((2-1) << RCC_PLL1DIVR_R1_Pos);

	// 4. Enable PLL1
	RCC->CR |= RCC_CR_PLL1ON;
	while ((RCC->CR & RCC_CR_PLL1RDY) == 0);

	// 5. Set AHB, APB1, APB2 prescalers (all to /2 for safety)
	RCC->D1CFGR = (RCC->D1CFGR & ~RCC_D1CFGR_HPRE_Msk) | (0x8 << RCC_D1CFGR_HPRE_Pos); // AHB /2
	RCC->D2CFGR = (RCC->D2CFGR & ~RCC_D2CFGR_D2PPRE1_Msk) | (0x4 << RCC_D2CFGR_D2PPRE1_Pos); // APB1 /2
	RCC->D2CFGR = (RCC->D2CFGR & ~RCC_D2CFGR_D2PPRE2_Msk) | (0x4 << RCC_D2CFGR_D2PPRE2_Pos); // APB2 /2

	// 6. Switch SYSCLK to PLL1
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= (0x3 << RCC_CFGR_SW_Pos); // 0x3 = PLL1
	while ((RCC->CFGR & RCC_CFGR_SWS) != (0x3 << RCC_CFGR_SWS_Pos));

	// 7. Update SystemCoreClock variable (if using CMSIS)
	SystemCoreClockUpdate(); // Uncomment if using CMSIS system file

	// 8. Enable UART and I2C peripheral clocks (example: USART2, I2C1)
	RCC->APB1LENR |= RCC_APB1LENR_USART2EN | RCC_APB1LENR_I2C1EN;
	// Add more as needed for your board
}
