/*
 * A simple board made for testing code rabbit that contains only a cpu, memory, and serial output
 */

#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "hw/core/boards.h"
#include "hw/mips/mips.h"
#include "system/address-spaces.h"
#include "system/reset.h"
#include "system/system.h"
#include "target/mips/cpu.h"

#define RABBIT_UART_BASE 0x1FD003F8
#define RABBIT_RAM_ID    "rabbit.ram"
#define TYPE_RABBIT "rabbit"

static void main_cpu_reset(void *opaque)
{
    MIPSCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

static void rabbit_init(MachineState *machine)
{
    MemoryRegion *system_memory = get_system_memory();
    MIPSCPU *cpu;
    CPUMIPSState *env;

    cpu = MIPS_CPU(cpu_create(machine->cpu_type));
    env = &cpu->env;

    cpu_mips_irq_init_cpu(cpu);
    cpu_mips_clock_init(cpu);
    qemu_register_reset(main_cpu_reset, cpu);

    memory_region_add_subregion(system_memory, 0x0, machine->ram);

    serial_mm_init(system_memory,
                   RABBIT_UART_BASE,
                   0,
                   env->irq[2],
                   115200,
                   serial_hd(0),
                   DEVICE_LITTLE_ENDIAN)
}

static void rabbit_machine_init(MachineClass *mc)
{
    mc->desc = "Rabbit board";
    mc->init = rabbit_init;
    mc->default_cpu_type = MIPS_CPU_TYPE_NAME("24Kf");
    mc->default_ram_size = 8 * MiB;
    mc->default_ram_id = RABBIT_RAM_ID;
}

DEFINE_MACHINE("rabbit", rabbit_machine_init)
