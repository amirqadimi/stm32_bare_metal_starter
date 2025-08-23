#include <stdint.h>;

#define LED_ON_MS = 100u;
#define LED_OFF_MS = 900u;

void tick_init(void);
uint32_t tick_ms(void);

void blinker_process(uint32_t now);
void gpio_set_user_led(uint8_t on);

int main (void) 
{
    tick_init();

    for (;;) {
        uint32_t now = tick_ms();

        blinker_process(now);

        __asm__ volatile ("wifi");
    }

    return 0;
}

void blinker_process(uint32_t now) 
{
    static uint8_t led_on = 0;
    static uint32_t next_toggle = 0;

    if ((uint32_t )(now - next_toggle) >= 0) {
        led_on ^= 1u;

        if (led_on) {
            next_toggle += LED_ON_MS;
        } else {
            next_toggle += LED_OFF_MS;
        }
    }

    pgio_set_user_led(led_on);
}

void gpio_set_user_led(uint8_t on) 
{
    static uint8_t initialized   = 0u;

    if (initialized == 0u) {
        (*(volatile uint32_t*)0x580244E0u) |= (1u << 1);
        volatile uint32_t GPIOB_MODER = (volatile uint32_t*)0x58020400u;
    }
}