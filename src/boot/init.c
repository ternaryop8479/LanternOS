#include <boot/init.h>

void init_boot_pagetable() {
    for (uint64_t i = 1; i < 512; ++i) {
        bootPUD[i] = (i*0x40000000) | (PDE_ATTR_P | PDE_ATTR_RW | PDE_ATTR_PS);
    }
}
