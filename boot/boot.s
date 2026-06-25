// =============================================================================
// boot.s - AArch64 Kernel Entry Point for Raspberry Pi 4
// =============================================================================

#include "arch/arm_regs.h"

.section ".text.boot"
.global _start

_start:
    // =========================================================================
    // STEP 1: Core Selection
    // Only Core 0 proceeds. All other cores sleep in a low-power loop.
    // MPIDR_EL1[7:0] holds the CPU affinity (core ID).
    // =========================================================================
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, check_el                // Core 0 proceeds
proc_hang:
    wfe
    b       proc_hang                   // All other cores spin here forever

    // =========================================================================
    // STEP 2: Exception Level Check & Drop to EL1
    // RPi4 firmware starts us at EL2. We must drop to EL1 before touching
    // any _el1 system registers, otherwise we get an illegal instruction fault.
    // =========================================================================
check_el:
    mrs     x0, CurrentEL
    lsr     x0, x0, #2                  // CurrentEL[3:2] holds the EL value
    cmp     x0, #2
    beq     drop_to_el1                 // If EL2, set up and drop down
    b       el1_entry                   // Already at EL1, continue

drop_to_el1:
    // Configure HCR_EL2: set RW bit (bit 31) so EL1 runs in AArch64 mode
    mrs     x0, hcr_el2
    orr     x0, x0, #(1 << 31)         // HCR_EL2.RW = 1 → EL1 is AArch64
    msr     hcr_el2, x0

    // Set SPSR_EL2 to return to EL1h (EL1 with its own SP) with interrupts
    // masked: D(9), A(8), I(7), F(6) masked, M[3:0] = 0101 (EL1h)
    mov     x0, #0x3C5
    msr     spsr_el2, x0

    // Set ELR_EL2 to where we want to resume (el1_entry) after eret
    adr     x0, el1_entry
    msr     elr_el2, x0

    // Exception Return: drops the CPU to EL1 and jumps to el1_entry
    eret

    // =========================================================================
    // STEP 3: EL1 Initialisation
    // =========================================================================
el1_entry:
    // -------------------------------------------------------------------------
    // 3a. Clear BSS Section
    // All zero-initialized global/static C variables live here.
    // If we skip this, they contain garbage → undefined behavior in kernel_main.
    // -------------------------------------------------------------------------
    adr     x0, __bss_start
    adr     x1, __bss_end
    sub     x1, x1, x0                  // x1 = size of BSS in bytes
    cbz     x1, bss_done                // Skip if BSS is empty
bss_loop:
    str     xzr, [x0], #8              // Write 8 bytes of zero, advance pointer
    subs    x1, x1, #8
    b.gt    bss_loop
bss_done:

    // -------------------------------------------------------------------------
    // 3b. Set Up the Stack Pointer
    // AArch64 stack grows DOWNWARD, so SP must point to the TOP of the
    // stack region (defined in the linker script), not _end.
    // Stack must be 16-byte aligned per AArch64 ABI.
    // -------------------------------------------------------------------------
    ldr     x0, =__stack_top            // Defined in your linker script
    and     x0, x0, #~0xF              // Ensure 16-byte alignment
    mov     sp, x0

    // -------------------------------------------------------------------------
    // 3c. Configure Memory Attributes (MAIR_EL1)
    // Defines the memory attribute slots referenced by page table entries.
    // -------------------------------------------------------------------------
    ldr     x0, =MAIR_VALUE
    msr     mair_el1, x0

    // -------------------------------------------------------------------------
    // 3d. Configure Translation Control Register (TCR_EL1)
    // Controls page table size, granule, and address space sizes.
    // -------------------------------------------------------------------------
    ldr     x0, =TCR_VALUE
    msr     tcr_el1, x0

    // Instruction Sync Barrier: ensure MAIR and TCR writes take effect
    // before we proceed to build or activate page tables.
    isb

    // -------------------------------------------------------------------------
    // 3e. Build the 4-Level Page Tables
    // mmu_init() populates l0_table → l1 → l2 → l3 descriptors in C.
    // Must complete BEFORE we load TTBR registers or enable the MMU.
    // -------------------------------------------------------------------------
    bl      mmu_init

    // -------------------------------------------------------------------------
    // 3f. Load Translation Table Base Registers
    //
    // TTBR0_EL1 → Lower virtual address space (0x0000_0000_0000_0000 ...)
    //             Used for user-space or identity-mapped regions.
    //
    // TTBR1_EL1 → Upper virtual address space (0xFFFF_0000_0000_0000 ...)
    //             Used when your kernel is linked to a high virtual address.
    //
    // Set BOTH. If your kernel lives at a high address and TTBR1 is not set,
    // the CPU faults immediately after MMU enable.
    // -------------------------------------------------------------------------
    ldr     x0, =l0_table
    msr     ttbr0_el1, x0               // Lower half page table

    ldr     x0, =l0_table_kern          // Your kernel-space L0 table
    msr     ttbr1_el1, x0               // Upper half page table

    // Data Sync Barrier: ensure all page table writes are visible to the
    // MMU hardware before we flip the enable bit.
    dsb     sy
    isb

    // =========================================================================
    // STEP 4: Enable the MMU + Caches (SCTLR_EL1)
    // =========================================================================

    // Read current SCTLR_EL1 value.
    // At reset, some bits are ARCHITECTURALLY UNKNOWN — mask them to a
    // known-safe baseline before OR-ing in our desired flags.
    mrs     x0, sctlr_el1
    ldr     x1, =SCTLR_SAFE_MASK        // Zero out reserved/unknown bits
    and     x0, x0, x1
    ldr     x1, =SCTLR_ENABLE_FLAGS     // MMU(0), D-Cache(2), I-Cache(12)
    orr     x0, x0, x1
    msr     sctlr_el1, x0               // Write: MMU and caches now ON

    // Full system barrier: flush all pending memory ops and pipeline.
    // From the next instruction onward, the CPU runs in virtual memory.
    dsb     sy
    isb

    // =========================================================================
    // STEP 5: Set Up Exception Vector Table
    // Must be set BEFORE jumping to C so any fault in kernel_main is caught.
    // VBAR_EL1 must be 2KB-aligned per AArch64 spec.
    // =========================================================================
    ldr     x0, =exception_vector_table
    msr     vbar_el1, x0
    isb

    // =========================================================================
    // STEP 6: Jump to Kernel Main
    // From here, we are fully in virtual memory with caches and exceptions.
    // =========================================================================
    bl      kernel_main

    // =========================================================================
    // STEP 7: Halt if kernel_main ever returns (it should never return)
    // =========================================================================
halt:
    wfe
    b       halt