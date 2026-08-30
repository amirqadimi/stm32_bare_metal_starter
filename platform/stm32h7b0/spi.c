/*	SPI2 master.

		PA9   SPI2_SCK    AF5
		PC1   SPI2_MOSI   AF5
		PC2_C SPI2_MISO   AF5
		PB12  CS          plain GPIO, active low (SSM=1)

	PC2_C is a dual analog/digital pad. Its analog switch is closed out of
	reset, so the driver only keeps SYSCFG_PMCR.PC2SO clear.
*/

#include "stm32h7b0xx.h"
#include <stddef.h>
#include <stdint.h>
#include "spi.h"

#define SPI_TIMEOUT 1000000UL

#define SPI_CS_PORT    GPIOB
#define SPI_CS_PIN     12U
#define SPI_AF         5U

static int wait_for_spi_flag(uint32_t flag)
{
	uint32_t timeout = SPI_TIMEOUT;

	while ((SPI2->SR & flag) == 0U)
	{
		if (timeout == 0U)
		{
			return -1;
		}

		timeout--;
	}

	return 0;
}

static void spi_cs_pin_config(GPIO_TypeDef *port, uint8_t pin)
{
	/* SSM=1, so CS is a plain GPIO driven low by hand. */
	port->MODER &= ~(3U << (pin * 2U));
	port->MODER |= (1U << (pin * 2U));

	port->OTYPER &= ~(1 << pin);

	port->OSPEEDR &= ~(3U << (pin * 2U));
	port->OSPEEDR |= (2U << (pin * 2U));

	port->PUPDR &= ~(3U << (pin * 2U));
}

static void spi_pin_config(GPIO_TypeDef *port, uint8_t pin)
{
	port->MODER &= ~(3U << (pin * 2U));
	port->MODER |= (2U << (pin * 2U));

	port->OTYPER &= ~(1 << pin);

	port->OSPEEDR &= ~(3U << (pin * 2U));
	port->OSPEEDR |= (2U << (pin * 2U));

	port->PUPDR &= ~(3U << (pin * 2U));

	uint8_t index = pin >> 3U;
	uint8_t shift = (pin & 0x7U) * 4U;

	port->AFR[index] &= ~(0xFU << shift);
	port->AFR[index] |= (SPI_AF << shift);
}

static void spi_enable(void)
{
	/* CS low. */
	SPI_CS_PORT->BSRR = (1UL << (GPIO_BSRR_BR0_Pos + SPI_CS_PIN));
	SPI2->CR1 |= SPI_CR1_SPE;
}

static void spi_disable(void)
{
	/* CS high. */
	SPI_CS_PORT->BSRR = (1UL << SPI_CS_PIN);
	SPI2->CR1 &= ~SPI_CR1_SPE;
}

int spi_init(void)
{
	RCC->CR |= RCC_CR_HSIKERON;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0U)
	{}

	RCC->CDCCIPR &= ~RCC_CDCCIPR_CKPERSEL_Msk;
	RCC->CDCCIP1R &= ~RCC_CDCCIP1R_SPI123SEL_Msk;
	RCC->CDCCIP1R |= RCC_CDCCIP1R_SPI123SEL_2;

	RCC->APB1LENR |= RCC_APB1LENR_SPI2EN;
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN |
					RCC_AHB4ENR_GPIOBEN |
					RCC_AHB4ENR_GPIOCEN;

	RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;

	(void)RCC->APB1LENR;
	(void)RCC->AHB4ENR;
	(void)RCC->APB4ENR;

	/* Keep the PC2_C analog switch closed so PC2 works as a digital pin. */
	SYSCFG->PMCR &= ~SYSCFG_PMCR_PC2SO;

	RCC->APB1LRSTR |= RCC_APB1LRSTR_SPI2RST;
	RCC->APB1LRSTR &= ~RCC_APB1LRSTR_SPI2RST;

	spi_cs_pin_config(SPI_CS_PORT, SPI_CS_PIN);
	spi_pin_config(GPIOA, 9);
	spi_pin_config(GPIOC, 2);
	spi_pin_config(GPIOC, 1);

	spi_disable();

	SPI2->CR1 |= SPI_CR1_SSI;

	SPI2->CFG1 |= (0x07UL << SPI_CFG1_DSIZE_Pos) |
				  (0x5UL << SPI_CFG1_MBR_Pos);

	SPI2->CFG2 |= SPI_CFG2_MASTER |
				 SPI_CFG2_SSM |
				 SPI_CFG2_AFCNTR;

	SPI2->IFCR = SPI_IFCR_EOTC |
				 SPI_IFCR_TXTFC |
				 SPI_IFCR_UDRC |
				 SPI_IFCR_OVRC |
				 SPI_IFCR_CRCEC |
				 SPI_IFCR_TIFREC |
				 SPI_IFCR_MODFC |
				 SPI_IFCR_TSERFC |
				 SPI_IFCR_SUSPC;

	return 0;
}

int spi_transfer(uint8_t *tx_data, uint8_t *rx_data, uint16_t size)
{
	size_t index;

	if (size == 0U)
	{
		return -1;
	}

	spi_disable();

	SPI2->IFCR = SPI_IFCR_EOTC |
				 SPI_IFCR_TXTFC |
				 SPI_IFCR_UDRC |
				 SPI_IFCR_OVRC |
				 SPI_IFCR_SUSPC;

	SPI2->CR2 &= ~SPI_CR2_TSIZE_Msk;
	SPI2->CR2 |= ((uint32_t)size << SPI_CR2_TSIZE_Pos);

	spi_enable();

	SPI2->CR1 |= SPI_CR1_CSTART;

	for (index = 0U; index < size; index++)
	{
		uint8_t tx_byte = 0xFFU;

		if (tx_data != 0)
		{
			tx_byte = tx_data[index];
		}

		if (wait_for_spi_flag(SPI_SR_TXP) != 0)
		{
			spi_disable();
			return -1;
		}

		*(volatile uint8_t *)&SPI2->TXDR = tx_byte;

		if (wait_for_spi_flag(SPI_SR_RXP) != 0)
		{
			spi_disable();
			return -1;
		}

		uint8_t rx_byte = *(volatile uint8_t *)&SPI2->RXDR;

		if (rx_data != 0)
		{
			rx_data[index] = rx_byte;
		}
	}

	if (wait_for_spi_flag(SPI_SR_EOT) != 0)
	{
		spi_disable();
		return -1;
	}

	spi_disable();

	SPI2->IFCR = SPI_IFCR_EOTC |
				 SPI_IFCR_TXTFC;

	return 0;
}
