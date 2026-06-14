# RPi OS - Bare-Metal Raspberry Pi Operating System

A comprehensive bare-metal operating system implementation for the Raspberry Pi 4 (AArch64 architecture) built from scratch in C and ARM64 Assembly.
This project demonstrates core OS concepts including bootloading, virtual memory management, exception handling, interrupt processing, and preemptive multitasking.

### Key Features

- **Bare-Metal Bootloader**: Custom boot sequence with BSS initialization and hardware setup
- **Virtual Memory (MMU)**: 4-level ARMv8 page table implementation with identity mapping
- **Exception Handling**: ARM64 exception vector table with IRQ/SVC support
- **Interrupt Processing**: System timer integration for 10ms tick generation
- **Preemptive Multitasking**: Context switching and round-robin scheduling
- **Hardware Drivers**: UART, GPIO, and Timer support
- **No Dependencies**: Zero external libraries; pure C and Assembly
