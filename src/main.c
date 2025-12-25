#include <stdint.h>

/* ------------------ RCC / GPIO addresses (STM32H7) ------------------ */
#define RCC_AHB4ENR      (*(volatile uint32_t*)0x580244E0u)
#define GPIOE_BASE       (0x58021000u)
#define GPIOE_MODER      (*(volatile uint32_t*)(GPIOE_BASE + 0x00u))
#define GPIOE_BSRR       (*(volatile uint32_t*)(GPIOE_BASE + 0x18u))

/* ------------------ SysTick (Cortex-M) ------------------ */
#define SYST_CSR         (*(volatile uint32_t*)0xE000E010u)
#define SYST_RVR         (*(volatile uint32_t*)0xE000E014u)
#define SYST_CVR         (*(volatile uint32_t*)0xE000E018u)
#define SYST_CSR_ENABLE  (1u << 0)
#define SYST_CSR_TICKINT (1u << 1)
#define SYST_CSR_CLKSRC  (1u << 2)

/* ------------------ LED: LD2 on PE1 ------------------ */
#define LED_GPIOE_EN     (1u << 4)   // RCC AHB4ENR bit for GPIOE
#define LED_PIN          (1u)        // PE1
#define LED_SET()        (GPIOE_BSRR = (1u << LED_PIN))
#define LED_RESET()      (GPIOE_BSRR = (1u << (LED_PIN + 16u)))
#define LED_TOGGLE()     do{ static uint8_t s; s^=1u; if(s) LED_SET(); else LED_RESET(); }while(0)

/* ------------------ millisecond timebase ------------------ */
static volatile uint32_t s_ms = 0u;

static void systick_init_1ms(void)
{
    /* Core clock assumed 64 MHz after reset -> 1ms = 64,000 cycles */
    SYST_RVR = 64000u - 1u;
    SYST_CVR = 0u;
    SYST_CSR = SYST_CSR_CLKSRC | SYST_CSR_TICKINT | SYST_CSR_ENABLE;
}

void SysTick_Handler(void)
{
    s_ms++;
}

static inline uint32_t millis(void) { return s_ms; }

static void delay_ms(uint32_t ms)
{
    uint32_t start = millis();
    while ((millis() - start) < ms) { __asm volatile("nop"); }
}

static void led_init(void)
{
    RCC_AHB4ENR |= LED_GPIOE_EN;

    uint32_t moder = GPIOE_MODER;
    moder &= ~(3u << (LED_PIN * 2u));   // clear
    moder |=  (1u << (LED_PIN * 2u));   // output (01)
    GPIOE_MODER = moder;

    LED_RESET(); // start off
}

int main(void)
{
    led_init();
    systick_init_1ms();

    for (;;)
    {
        LED_TOGGLE();
        delay_ms(500);
    }
}
