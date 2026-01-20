#include "stm32f4xx.h"

#define LED_DELAY_COUNT 1000000U

static void delay(volatile uint32_t count)
{
    while (count--) {
        __asm__("nop");
    }
}

int main(void)
{
    // Enable clock for GPIOD
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    // Configure PD14 as general purpose output
    GPIOD->MODER &= ~(0x3U << (14U * 2U)); // clear mode bits
    GPIOD->MODER |=  (0x1U << (14U * 2U)); // set to output mode

    // Output type: push-pull
    GPIOD->OTYPER &= ~(0x1U << 14U);

    // Output speed: medium
    GPIOD->OSPEEDR |=  (0x1U << (14U * 2U));
    GPIOD->OSPEEDR &= ~(0x3U << (14U * 2U));

    // No pull-up, no pull-down
    GPIOD->PUPDR &= ~(0x3U << (14U * 2U));

    while (1) {
        // Set PD14
        GPIOD->BSRR = (0x1U << 14U);
        delay(LED_DELAY_COUNT);

        // Reset PD14
        GPIOD->BSRR = (0x1U << (14U + 16U));
        delay(LED_DELAY_COUNT);
    }

    return 0;
}
