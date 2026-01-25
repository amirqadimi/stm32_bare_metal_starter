#include "led.h"

#if defined(STM32H723xx)
#include "stm32h7xx.h"

void led_init(void)
{
    // Enable clock for GPIOB
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;

    // Configure PB0 as general purpose output
    GPIOB->MODER &= ~(0x3U << (0U * 2U)); // clear mode bits
    GPIOB->MODER |=  (0x1U << (0U * 2U)); // set to output mode

    // Output type: push-pull
    GPIOB->OTYPER &= ~(0x1U << 0U);

    // Output speed: medium
    GPIOB->OSPEEDR |=  (0x1U << (0U * 2U));
    GPIOB->OSPEEDR &= ~(0x3U << (0U * 2U));

    // No pull-up, no pull-down
    GPIOB->PUPDR &= ~(0x3U << (0U * 2U));
    
    // Start with LED off
    led_off();
}

void led_on(void)
{
    // Set PB0
    GPIOB->BSRR = (0x1U << 0U);
}

void led_off(void)
{
    // Reset PB0
    GPIOB->BSRR = (0x1U << (0U + 16U));
}

void led_toggle(void)
{
    // Read current state and toggle
    if (GPIOB->ODR & (0x1U << 0U)) {
        led_off();
    } else {
        led_on();
    }
}

#endif