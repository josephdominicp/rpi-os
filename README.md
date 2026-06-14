# RPi OS - Bare-Metal Raspberry Pi Operating System

A comprehensive bare-metal operating system implementation for the Raspberry Pi 4 (AArch64 architecture) built from scratch in C and ARM64 Assembly. This project demonstrates core OS concepts including bootloading, virtual memory management, exception handling, interrupt processing, and preemptive multitasking.

## Key Features (10 Points)

1. **Bare-Metal Bootloader (ARM64 Assembly)**
   - Custom boot sequence with primary/secondary CPU core distinction using `mpidr_el1` register
   - BSS section initialization to zero for data integrity
   - Stack pointer setup at `0x80000` before C kernel entry
   - No dependencies on firmware or operating system—runs directly on hardware

2. **Memory Attributes & Control Configuration**
   - MAIR (Memory Attribute Indirection Register) configuration for proper memory type handling
   - TCR (Translation Control Register) setup for correct MMU behavior
   - Proper memory barriers (`dsb`, `isb`) to ensure architectural correctness

3. **4-Level ARMv8 Page Table Implementation (MMU)**
   - Full identity mapping with L0 → L3 page table hierarchy
   - 4KB page granule alignment using linker script sections
   - Page table base address (L0 table) registered in `ttbr0_el1`
   - Supports modern ARM64 virtual memory addressing schemes

4. **Virtual Memory Activation**
   - SCTLR_EL1 register configuration to enable MMU, data cache, and instruction cache
   - Transition from physical to virtual memory addressing post-boot
   - All subsequent kernel execution runs in virtual address space

5. **ARM64 Exception Handling**
   - Exception vector table with dedicated handlers for different exception types
   - Vector Base Address Register (VBAR_EL1) configuration
   - Foundation for IRQ, SVC, and abort exception processing
   - Proper alignment and positioning of exception handlers

6. **Interrupt Processing & Timer Integration**
   - ARM Generic Timer programmed for 10ms tick intervals
   - IRQ mask control via DAIF register (`daifclr` for unmasking)
   - Low-power mode support using `wfi` (Wait For Interrupt) instruction
   - Hardware-driven context switching based on timer ticks

7. **Preemptive Multitasking Scheduler**
   - Round-robin task scheduling with 10ms quantum
   - Task context control block infrastructure (`task_t` structures)
   - `task_create()` and `task_init()` APIs for task management
   - Support for multiple independent user-space tasks plus idle task

8. **Hardware Drivers**
   - **UART Driver**: Serial console for diagnostic output and logging (`uart_init()`, `uart_puts()`)
   - **GPIO Support**: Foundation for peripheral control and LED/button interaction
   - **Timer Driver**: ARM Generic Timer configuration and interrupt routing
   - All drivers built from bare-metal register access—no abstractions

9. **Linker Script & Build System**
   - Custom linker script (`linker.ld`) defining memory layout and sections
   - Makefile-based cross-compilation using `aarch64-linux-gnu-` toolchain
   - Separate compilation of Assembly (`.S`) and C (`.c`) sources with proper flags
   - Binary kernel image generation (`kernel8.img` for RPi4) via `objcopy`

10. **Zero External Dependencies**
    - Pure C and Assembly implementation—no libc, standard library, or runtime
    - Compiler flags: `-ffreestanding -nostdinc -nostdlib -nostartfiles`
    - Complete control over memory layout, execution flow, and hardware interaction
    - Minimal kernel—only essential subsystems to demonstrate OS concepts

## Project Structure

```
rpi-os/
├── boot/
│   ├── boot.S              # Primary bootloader (ARM64 assembly)
│   └── vectors.s           # Exception vector table
├── src/
│   ├── kernal.c            # Main kernel entry point
│   ├── arch/               # Architecture-specific code
│   ├── mmu/                # Virtual memory implementation
│   ├── sched/              # Task scheduler
│   ├── drivers/            # Hardware drivers (UART, GPIO, Timer)
│   └── sys/                # System calls and exceptions
├── include/                # Header files
├── linker.ld               # Linker script for memory layout
├── Makefile                # Build configuration
├── kernel8.elf             # ELF executable (debug symbols)
└── kernel8.img             # Binary kernel image (RPi4 bootable)
```

## Building

```bash
# Prerequisites: aarch64-linux-gnu cross-compilation toolchain
make clean
make
```

This generates `kernel8.img` ready for Raspberry Pi 4 SD card deployment.

## Getting Started

1. Copy `kernel8.img` to the boot partition of your RPi 4 SD card
2. Boot the device and observe UART output via serial console (115200 baud)
3. The kernel will initialize MMU, enable interrupts, and start executing tasks

## Technical Highlights

- **10ms Preemption**: Timer-driven context switching ensures fairness
- **Identity Mapping**: Simplified virtual-to-physical address translation
- **Low-Power Support**: CPU sleeps via `wfi` when no tasks ready
- **Debug Logging**: UART provides real-time visibility into OS operations

## Learning Value

This project is ideal for understanding:
- ARM64 architecture and AArch64 instruction set
- Bare-metal bootloader design
- Virtual memory and MMU concepts
- Interrupt and exception handling
- Kernel-space multitasking and scheduling
- Hardware driver development

---

**Author**: josephdominicp  
**Target**: Raspberry Pi 4 (AArch64)  
**Language**: C + ARM64 Assembly  
**License**: [Specify your license]
