#include "timer.h"
#include "arch/i686/io.h"
#include <stdint.h>
#include <arch/i686/irq.h>
#include <stdatomic.h>

#define PIT_CHANNEL_0_DATA_PORT 0x40
#define PIT_COMMAND_PORT 0x43
#define PIT_INPUT_CLOCK_FREQUENCY 1193182

volatile atomic_uint_fast64_t g_ticks = 0;

void interruptPIT(Registers* _) {
    atomic_fetch_add(&g_ticks, 1);
}

void pit_set_frequency(uint32_t frequency) {
    uint16_t divisor = PIT_INPUT_CLOCK_FREQUENCY / frequency;
    i686_outb(PIT_COMMAND_PORT, 0x36); // Set command byte to 0x36 (channel 0, lobyte/hibyte, mode 3)
    i686_outb(PIT_CHANNEL_0_DATA_PORT, divisor & 0xFF); // Send low byte of divisor
    i686_outb(PIT_CHANNEL_0_DATA_PORT, (divisor >> 8) & 0xFF); // Send high byte of divisor
}

static inline uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ volatile("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

uint64_t cpu_frequency = 0; // Set this to the actual CPU frequency of your system

void calibrate_cpu_frequency() {
    pit_set_frequency(100); // Set the PIT frequency to 100 Hz
    i686_IRQ_RegisterHandler(0, interruptPIT);

    uint64_t start = rdtsc();
    uint64_t start_ticks = atomic_load(&g_ticks);

    while(atomic_load(&g_ticks) < start_ticks + 100);

    uint64_t end = rdtsc();
    cpu_frequency = (end - start) / 100;
}

uint64_t getTime() {
    if(!cpu_frequency) {
        calibrate_cpu_frequency();
    }

    return rdtsc();
}

uint64_t getNanos() {
    return getTime() * 1000000000 / cpu_frequency;
}

uint64_t getMillis() {
    return getTime() * 1000 / cpu_frequency;
}

uint64_t getMicros() {
    return getTime() * 1000000 / cpu_frequency;
}
