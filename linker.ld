SECTIONS
{
    /* ... Your existing .text and .data sections ... */
    . = 0x80000;
    .text.boot : { KEEP(*(.text.boot)) }
    .text       : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }

    /* Ensure BSS is aligned */
    . = ALIGN(16);
    __bss_start = .;
    .bss : { *(.bss) }
    __bss_end = .;

    /* ==============================================
       DAY 1 ADDITION: 4KB Aligned Page Table Section
       ============================================== */
    . = ALIGN(4096);
    __page_tables_start = .;
    
    .pgtbl : {
        *(.pgtbl)
    }
    
    . = ALIGN(4096);
    __page_tables_end = .;

    /* End of the kernel image space */
    _end = .;
}