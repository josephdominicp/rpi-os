#include "mmu.h"

// Allocate our structural tables
__attribute__((section(".pgtbl"))) uint64_t l0_table[PTRS_PER_TABLE];
__attribute__((section(".pgtbl"))) uint64_t l1_table[PTRS_PER_TABLE];
__attribute__((section(".pgtbl"))) uint64_t l2_table[PTRS_PER_TABLE];

// Allocate TWO distinct Level 3 tables (4KB each)
__attribute__((section(".pgtbl"))) uint64_t l3_ram_table[PTRS_PER_TABLE];   // For RAM (0x0+)
__attribute__((section(".pgtbl"))) uint64_t l3_dev_table[PTRS_PER_TABLE];   // For Peripherals (0xFE000000+)

void mmu_init(void) {
    int i;

    // 1. Sanitize all tables
    for (i = 0; i < PTRS_PER_TABLE; i++) {
        l0_table[i] = 0; l1_table[i] = 0; l2_table[i] = 0;
        l3_ram_table[i] = 0; l3_dev_table[i] = 0;
    }

    // 2. Connect L0 -> L1 -> L2
    l0_table[0] = ((uint64_t)l1_table) | MM_TYPE_PAGE_TABLE;
    l1_table[0] = ((uint64_t)l2_table) | MM_TYPE_PAGE_TABLE;

    // 3. Connect L2 slots to our TWO different L3 tables
    // Slot 0 handles addresses from 0 to 1GB
    l2_table[0] = ((uint64_t)l3_ram_table) | MM_TYPE_PAGE_TABLE;
    
    // On the RPi 3, the peripheral region containing GPIO and Mini UART starts at 0x3F200000.
    // Index 505 of the L2 table handles the 2MB address range 0x3F200000 to 0x3F3FFFFF.
    l2_table[505] = ((uint64_t)l3_dev_table) | MM_TYPE_PAGE_TABLE;

    // 4. Populate the RAM L3 Table (Maps 0x0 to 2MB)
    uint64_t ram_address = 0;
    for (i = 0; i < PTRS_PER_TABLE; i++) {
        l3_ram_table[i] = ram_address | MMU_FLAGS_NORMAL;
        ram_address += PAGE_SIZE;
    }

    // 5. Populate the Peripheral L3 Table (Maps 0x3F200000 to 0x3F400000)
    // This perfectly covers GPIO and Mini UART (AUX) registers.
    uint64_t dev_address = 0x3F200000;
    for (i = 0; i < PTRS_PER_TABLE; i++) {
        l3_dev_table[i] = dev_address | MMU_FLAGS_DEVICE;
        dev_address += PAGE_SIZE;
    }
}