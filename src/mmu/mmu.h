#ifndef _MMU_H
#define _MMU_H

#include <stdint.h>

// 4KB Page size allocation parameters
#define PAGE_SHIFT      12
#define PAGE_SIZE       (1 << PAGE_SHIFT)

// Entry count per table (4KB granule / 8 bytes per entry = 512 entries)
#define PTRS_PER_TABLE  512

// Page Table Entry bit configurations
#define MM_TYPE_PAGE_TABLE   0x3   // Table descriptor valid bit
#define MM_TYPE_PAGE         0x3   // Level 3 block attribute valid bit
#define MM_ACCESS_FLAG       (1 << 10) // Access flag (Must be 1 to prevent faults)
#define MM_ATTR_DEVICE_INDEX 0     // Matches MAIR_DEVICE_nGnRnE attribute slot
#define MM_ATTR_NORMAL_INDEX 1     // Matches MAIR_NORMAL_WB_WA attribute slot

// Combined flags for Table descriptors and Block mappings
#define MMU_FLAGS_DEVICE     (MM_TYPE_PAGE | (MM_ATTR_DEVICE_INDEX << 2) | MM_ACCESS_FLAG)
#define MMU_FLAGS_NORMAL     (MM_TYPE_PAGE | (MM_ATTR_NORMAL_INDEX << 2) | MM_ACCESS_FLAG)


// Function prototype declaration
void mmu_init(void);

#endif