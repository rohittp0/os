#pragma once

#include <stdint.h>
#include "arch/i686/irq.h"

uint64_t getMillis();
void timer(Registers* _);
void pit_set_frequency(uint32_t frequency);
