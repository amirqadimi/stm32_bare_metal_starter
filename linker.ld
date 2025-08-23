/* STM32H723ZG minimal linker script */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
  DTCM  (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

_estack = ORIGIN(DTCM) + LENGTH(DTCM);  /* top of stack */

SECTIONS
{
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    *(.text*) *(.rodata*) *(.glue_7) *(.glue_7t)
    _etext = .;
  } > FLASH

  /* Initialized data in RAM with LMA in FLASH */
  .data : AT(_etext)
  {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > DTCM

  /* BSS */
  .bss (NOLOAD) :
  {
    _sbss = .;
    *(.bss*) *(COMMON)
    _ebss = .;
  } > DTCM

  /* For copying .data */
  _sidata = LOADADDR(.data);
}
