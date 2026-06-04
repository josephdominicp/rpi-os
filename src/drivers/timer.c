#include <stdint.h>

// Global operating system heartbeat tracker
volatile uint64_t system_ticks = 0;

static uint32_t clock_frequency = 0;
static uint32_t ticks_per_10ms  = 0;

// --- STEP 1: INITIALIZE THE SYSTEM TIMER ---
void timer_init(void) {
    // 1. Read the hardware crystal oscillator frequency (ticks per second).
    // On the Raspberry Pi 4, this register typically reports 54,000,000 Hz.
    asm volatile("mrs %0, cntfrq_el0" : "=r"(clock_frequency));
    
    // 2. Calculate the precise number of ticks needed for a 10ms (100Hz) slice.
    // Example: 54,000,000 / 100 = 540,000 ticks.
    ticks_per_10ms = clock_frequency / 100;
    
    // 3. Load the countdown value into the Timer Value register.
    // The hardware will decrement this register automatically on every crystal pulse.
    asm volatile("msr cntp_tval_el0, %0" :: "r"(ticks_per_10ms));
    
    // 4. Enable the physical timer control register.
    // Bit 0 = 1: Enable Timer
    // Bit 1 = 0: Unmask Interrupt output (allows the signal to reach the CPU interrupt controller)
    uint64_t control = 1;
    asm volatile("msr cntp_ctl_el0, %0" :: "r"(control));
}

// --- STEP 2: THE INTERRUPT SERVICE ROUTINE (ISR) ---
// This function will be called directly from our low-level assembly 
// exception handlers whenever the timer hits zero.
// Declare the external scheduler hook
extern void timer_tick(void);

void handle_timer_irq(void) {
    // 1. Reload the hardware countdown register for the next 10ms (From Day 9)
    asm volatile("msr cntp_tval_el0, %0" :: "r"(ticks_per_10ms));
    
    // 2. Increment global uptime counter
    system_ticks++;
    
    // 3. THE LIVE TRIGGER: Tell the scheduler a 10ms tick just passed!
    timer_tick();
}