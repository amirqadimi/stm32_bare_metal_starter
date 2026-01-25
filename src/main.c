#include "led.h"
#include <inttypes.h>

#define LED_DELAY_COUNT 1000000U

static void delay(volatile uint32_t count)
{
    while (count--) {
        __asm__("nop");
    }
}

int main(void)
{
    led_init();

    while (1) {
        led_on();
        delay(LED_DELAY_COUNT);

        led_off();
        delay(LED_DELAY_COUNT);
    }

    return 0;
}
