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
    // Initialize the LED
    led_init();

    while (1) {
        // Turn LED on
        led_on();
        delay(LED_DELAY_COUNT);

        // Turn LED off
        led_off();
        delay(LED_DELAY_COUNT);
    }

    return 0;
}
