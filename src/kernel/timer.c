#include "timer.h"
#include "arch/i686/io.h"
#include <stdatomic.h>

#define PIT_CHANNEL_0_DATA_PORT 0x40
#define PIT_COMMAND_PORT 0x43
#define PIT_INPUT_CLOCK_FREQUENCY 1193182

atomic_uint_fast64_t g_ticks = 0;

void timer(Registers* _)
{
    atomic_fetch_add(&g_ticks, 1);
}

uint64_t getMillis()
{
    return atomic_load(&g_ticks);
}

void pit_set_frequency(uint32_t frequency) {
    uint16_t divisor = PIT_INPUT_CLOCK_FREQUENCY / frequency;
    i686_outb(PIT_COMMAND_PORT, 0x36); // Set command byte to 0x36 (channel 0, lobyte/hibyte, mode 3)
    i686_outb(PIT_CHANNEL_0_DATA_PORT, divisor & 0xFF); // Send low byte of divisor
    i686_outb(PIT_CHANNEL_0_DATA_PORT, (divisor >> 8) & 0xFF); // Send high byte of divisor
}
