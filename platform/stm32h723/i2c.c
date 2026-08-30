/*	I2C2 master at 100 kHz, clocked from the 64 MHz HSI.

		PB10  I2C2_SCL  AF4, needs an external pull-up
		PB11  I2C2_SDA  AF4, needs an external pull-up
*/

#include "stm32h723xx.h"
#include "i2c.h"
#include <stddef.h>

#define I2C_INSTANCE I2C2
#define I2C_7BIT_ADDRESS_MAX 0x7FU

int i2c_init(void)
{
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
	(void)RCC->AHB4ENR;

	GPIOB->MODER &= ~(GPIO_MODER_MODER10_Msk |
					  GPIO_MODER_MODER11_Msk);

	GPIOB->MODER |= GPIO_MODER_MODER10_1 |
					GPIO_MODER_MODER11_1;

	GPIOB->OTYPER |= GPIO_OTYPER_OT10 |
					 GPIO_OTYPER_OT11;

	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED10_Msk |
						GPIO_OSPEEDR_OSPEED11_Msk);

	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED10_1 |
					  GPIO_OSPEEDR_OSPEED11_1;

	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD10_Msk |
					  GPIO_PUPDR_PUPD11_Msk);

	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL10_Msk |
					   GPIO_AFRH_AFSEL11_Msk);

	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL10_2 |
					 GPIO_AFRH_AFSEL11_2;

	RCC->APB1LENR |= RCC_APB1LENR_I2C2EN;
	(void)RCC->APB1LENR;

	RCC->CR |= RCC_CR_HSIKERON;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0U)
	{}

	RCC->D2CCIP2R &= ~RCC_D2CCIP2R_I2C1235SEL_Msk;
	RCC->D2CCIP2R |= RCC_D2CCIP2R_I2C1235SEL_1;

	I2C_INSTANCE->CR1 = 0;
	I2C_INSTANCE->CR2 = I2C_CR2_AUTOEND;

	I2C_INSTANCE->TIMINGR = 0;

	/* TIMINGR from CubeMX: 64 MHz kernel clock, 100 kHz standard mode. */
	I2C_INSTANCE->TIMINGR |= (0x1UL << I2C_TIMINGR_PRESC_Pos) |
					 (0x7UL << I2C_TIMINGR_SCLDEL_Pos) |
					 (0x0UL << I2C_TIMINGR_SDADEL_Pos) |
					 (0x7DUL << I2C_TIMINGR_SCLH_Pos) |
					 (0xBCUL << I2C_TIMINGR_SCLL_Pos);

	I2C_INSTANCE->CR1 |= I2C_CR1_PE;

	return 0;
}

static int check_error_flags(void)
{
	if ((I2C_INSTANCE->ISR & I2C_ISR_BERR) != 0U)
	{
		I2C_INSTANCE->ICR = I2C_ICR_BERRCF;
		return -1;
	}

	if ((I2C_INSTANCE->ISR & I2C_ISR_ARLO) != 0U)
	{
		I2C_INSTANCE->ICR = I2C_ICR_ARLOCF;
		return -1;
	}

	if ((I2C_INSTANCE->ISR & I2C_ISR_NACKF) != 0U)
	{
		I2C_INSTANCE->ICR = I2C_ICR_NACKCF;
		return -1;
	}

	return 0;
}

static int i2c_transmit(i2c_config_t *config, int direction)
{
	if (
		(config == NULL) ||
		(config->buff_ptr == NULL) ||
		(config->size == 0U) ||
		(config->address_7bit > I2C_7BIT_ADDRESS_MAX) ||
		((I2C_INSTANCE->CR1 & I2C_CR1_PE) == 0U) ||
		((I2C_INSTANCE->ISR & I2C_ISR_BUSY) != 0U)
	)
	{
		return -1;
	}

	I2C_INSTANCE->CR2 = I2C_CR2_AUTOEND;
	I2C_INSTANCE->CR2 |= ((config->address_7bit << 1) << I2C_CR2_SADD_Pos) |
						 (config->size << I2C_CR2_NBYTES_Pos);

	int is_write = direction == 0U;

	if (is_write)
	{
		I2C_INSTANCE->CR2 &= ~I2C_CR2_RD_WRN_Msk;
	}
	else
	{
		I2C_INSTANCE->CR2 |= I2C_CR2_RD_WRN;
	}

	I2C_INSTANCE->CR2 |= I2C_CR2_START;

	for (int index = 0; index < config->size; index++)
	{

		if (is_write)
		{
			while ((I2C_INSTANCE->ISR & I2C_ISR_TXIS) == 0U)
			{
				if (check_error_flags() != 0U)
				{
					return -1;
				}
			}

			I2C_INSTANCE->TXDR = config->buff_ptr[index];
		}

		if (check_error_flags() != 0U)
		{
			return -1;
		}

		if (is_write == 0)
		{
			while ((I2C_INSTANCE->ISR & I2C_ISR_RXNE) == 0U)
			{
				if (check_error_flags() != 0U)
				{
					return -1;
				}
			}

			config->buff_ptr[index] = (uint8_t)I2C_INSTANCE->RXDR;
		}
	}

	while ((I2C_INSTANCE->ISR & I2C_ISR_BUSY) != 0U)
	{
		if (check_error_flags() != 0U)
		{
			return -1;
		}
	}

	if ((I2C_INSTANCE->ISR & I2C_ISR_STOPF) != 0U)
	{
		I2C_INSTANCE->ICR = I2C_ICR_STOPCF;
	}

	return 0;
}

int i2c_write(i2c_config_t *config)
{
	if (i2c_transmit(config, 0) != 0)
	{
		return -1;
	}

	return 0;
}

int i2c_read(i2c_config_t *config)
{
	if (i2c_transmit(config, 1) != 0)
	{
		return -1;
	}

	return 0;
}