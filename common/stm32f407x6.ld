/* Linker script for the STM32F407VGT6 chip (1024K flash, 192K RAM). */

/* Define memory regions. */
MEMORY
{
    rom (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
    
    /* Put RAM in SRAM1: */
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 112K
}

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld
