#include "led.h"

#if defined(STM32F407xx)
#include "stm32f4xx.h"

void led_init(void)
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
    
    // Start with LED off
    led_off();
}

void led_on(void)
{
    // Set PD14
    GPIOD->BSRR = (0x1U << 14U);
}

void led_off(void)
{
    // Reset PD14
    GPIOD->BSRR = (0x1U << (14U + 16U));
}

void led_toggle(void)
{
    // Read current state and toggle
    if (GPIOD->ODR & (0x1U << 14U)) {
        led_off();
    } else {
        led_on();
    }
}

#endif