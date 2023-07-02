#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>
#include <arch/i686/irq.h>
#include <debug.h>
#include <boot/bootparams.h>
#include "timer.h"
#include "network.h"

extern void _init();

uint64_t g_ticks = 0;

void timer(Registers* regs)
{
    g_ticks++;
}

uint64_t getMillis()
{
    return g_ticks;
}

void start(BootParams* bootParams)
{
    // call global constructors
    _init();

    HAL_Initialize();

    log_debug("Main", "Boot device: %x", bootParams->BootDevice);
    log_debug("Main", "Memory region count: %d", bootParams->Memory.RegionCount);
    for (int i = 0; i < bootParams->Memory.RegionCount; i++)
    {
        log_debug("Main", "MEM: start=0x%llx length=0x%llx type=%x",
            bootParams->Memory.Regions[i].Begin,
            bootParams->Memory.Regions[i].Length,
            bootParams->Memory.Regions[i].Type);
    }

    i686_IRQ_RegisterHandler(0, timer);

    puts("\r\n\r\n\r\n");
    puts("       +=====================+\r\n");
    puts("       ||                   ||\r\n");
    puts("       ||    Not Windows    ||\r\n");
    puts("       ||                   ||\r\n");
    puts("       +=====================+\r\n");
    puts("\r\n\r\n\r\n");

    main();

end:
    for (;;);
}
