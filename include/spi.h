#ifndef SPI_H
#define SPI_H

#include <stdint.h>

int spi_init(void);

/*	tx_data may be NULL to clock out 0xFF, rx_data may be NULL to discard the
	bytes read back. Both NULL is allowed and just clocks the bus.
*/
int spi_transfer(uint8_t *tx_data, uint8_t *rx_data, uint16_t size);

#endif
