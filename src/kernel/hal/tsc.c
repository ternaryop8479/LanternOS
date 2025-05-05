#include <kernel/stdio.h>
#include <kernel/hal/tsc.h>

uint64_t cpu_count_per_nanosecond = 0;

KERNEL_STATUS tsc_init(EFI_SYSTEM_TABLE *systemTable, uint64_t calibrate_microseconds) {
    printlogf("[tsc_init()] Calculating CPU frequency(%ulldus)...", calibrate_microseconds);
    uint64_t startTSC = read_tsc();
    systemTable->BootServices->Stall(calibrate_microseconds);
    uint64_t endTSC = read_tsc();

    cpu_count_per_nanosecond = (endTSC - startTSC) / (calibrate_microseconds * 1024);
    printlogf("[tsc_init()] CPU count per nanosecond: %ulld", cpu_count_per_nanosecond);

    if(!check_invariant_tsc()) {
        printlogf("[tsc_init()][WARN] The CPU did not support invariant TSC.");
    }
    return KERNEL_SUCCESS;
}

bool check_invariant_tsc() {
    uint32_t eax, edx;
    eax = 0x80000007;
    asm volatile("cpuid" : "=a"(eax), "=d"(edx) : "a"(eax));
    return (edx >> 8) & 0x1; // Check the 8th bit of edx
}

uint64_t read_tsc() {
    uint32_t eax, edx;
    __asm__ volatile("rdtsc" : "=a"(eax), "=d"(edx));
    return ((uint64_t)edx << 32) | eax;
}

uint64_t tsc_to_nanosecond(uint64_t tsc) {
    return tsc / cpu_count_per_nanosecond;
}
