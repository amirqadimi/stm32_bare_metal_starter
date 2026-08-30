#ifndef I2CH
#define I2CH

#include <stdint.h>

int i2c_init(void);

typedef struct
{
	uint8_t address_7bit;
	uint8_t *buff_ptr;
	uint8_t size;
} i2c_config_t;

int i2c_write(i2c_config_t *config);
int i2c_read(i2c_config_t *config);

#endif