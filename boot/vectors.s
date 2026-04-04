    .section .vectors, "ax"
    .align 7
    .global vectors_start

/*
 * ARM Exception Vector Table (AArch32)
 *
 * Offset | Exception
 * ----------------------------
 * 0x00   | Reset
 * 0x04   | Undefined Instruction
 * 0x08   | Software Interrupt (SVC)
 * 0x0C   | Prefetch Abort
 * 0x10   | Data Abort
 * 0x14   | Reserved
 * 0x18   | IRQ
 * 0x1C   | FIQ
 */

vectors_start:
    b   reset_handler
    b   undef_handler
    b   svc_handler
    b   pabort_handler
    b   dabort_handler
    b   reserved_handler
    b   irq_handler
    b   fiq_handler

/*
 * ==========================
 * Exception Handlers
 * ==========================
 * For now, they just loop forever.
 * Later, IRQ/SVC will call C handlers.
 */

reset_handler:
    b   reset_handler

undef_handler:
    b   undef_handler

svc_handler:
    b   svc_handler

pabort_handler:
    b   pabort_handler

dabort_handler:
    b   dabort_handler

reserved_handler:
    b   reserved_handler

irq_handler:
    b   irq_handler

fiq_handler:
    b   fiq_handler