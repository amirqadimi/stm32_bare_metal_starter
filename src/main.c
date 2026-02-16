#include <inttypes.h>
#include "system_clock.h"

#define LED_DELAY_COUNT 1000000U

static void delay(volatile uint32_t count)
{
    while (count--) {
        __asm__("nop");
    }
}

int main(void)
{
    SystemClockConfig();
    
    return 0;
}
