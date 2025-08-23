#include <stdint.h>

/* Symbols from linker script */
extern unsigned long _estack;
extern unsigned long _sidata, _sdata, _edata;
extern unsigned long _sbss, _ebss;

int main(void);
void Reset_Handler(void);

/* Weak default handlers */
void Default_Handler(void) __attribute__((noreturn));
void NMI_Handler(void)            __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)      __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)      __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)            __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)         __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)        __attribute__((weak, alias("Default_Handler"))); // you override this in main.c

/* Vector table placed at start of Flash */
__attribute__((section(".isr_vector")))
const void *vector_table[] = {
    (void*)&_estack,        /* Initial MSP value */
    Reset_Handler,          /* Reset */
    NMI_Handler,            /* NMI */
    HardFault_Handler,      /* HardFault */
    MemManage_Handler,      /* MemManage */
    BusFault_Handler,       /* BusFault */
    UsageFault_Handler,     /* UsageFault */
    0, 0, 0, 0,             /* Reserved */
    SVC_Handler,            /* SVC */
    DebugMon_Handler,       /* Debug monitor */
    0,                      /* Reserved */
    PendSV_Handler,         /* PendSV */
    SysTick_Handler,        /* SysTick */
    /* You can append IRQ handlers here if/when you need them */
};

void Reset_Handler(void)
{
    /* Copy .data from Flash to RAM */
    unsigned long *src = &_sidata;
    unsigned long *dst = &_sdata;
    while (dst < &_edata) { *dst++ = *src++; }

    /* Zero .bss */
    for (dst = &_sbss; dst < &_ebss; ) { *dst++ = 0; }

    /* Optionally set VTOR to the Flash base (0x08000000) — H7 defaults there.
       If you later move the vector table to RAM, write SCB->VTOR. */

    /* Call main */
    (void)main();

    /* If main returns, trap */
    while (1) { __asm volatile("wfi"); }
}

void Default_Handler(void)
{
    while (1) { __asm volatile("wfi"); }
}
