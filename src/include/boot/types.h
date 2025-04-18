#ifndef LanternOS_Header_BOOT_TYPES_H
#define LanternOS_Header_BOOT_TYPES_H

#define CR0_PG (1 << 31)

#define CR4_PSE (1 << 4)
#define CR4_PAE (1 << 5)
#define CR4_PGE (1 << 7)

#define SELECTOR_RPL (0)
#define SELECTOR_TI (2)
#define SELECTOR_INDEX (3)

#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE - 1))

#define PTE_ATTR_P     (1 << 0)
#define PTE_ATTR_RW    (1 << 1)
#define PTE_ATTR_US    (1 << 2)
#define PTE_ATTR_PWT   (1 << 3)
#define PTE_ATTR_PCD   (1 << 4)
#define PTE_ATTR_A     (1 << 5)
#define PTE_ATTR_D     (1 << 6)

#define PDE_ATTR_P     (1 << 0)
#define PDE_ATTR_RW    (1 << 1)
#define PDE_ATTR_PS    (1 << 7)
#define PDE_DEFAULT     (PDE_ATTR_P | PDE_ATTR_RW)

#define PTE_OFFSET 12
#define PMD_OFFSET 21
#define PUD_OFFSET 30
#define PGD_OFFSET 39

#define PT_ENTRY_MASK 0b111111111UL
#define PTE_MASK (PT_ENTRY_MASK << PTE_OFFSET)
#define PMD_MASK (PT_ENTRY_MASK << PMD_OFFSET)
#define PUD_MASK (PT_ENTRY_MASK << PUD_OFFSET)
#define PGD_MASK (PT_ENTRY_MASK << PGD_OFFSET)

#define PTE_ENTRY(addr) ((addr >> PTE_OFFSET) & PT_ENTRY_MASK)
#define PMD_ENTRY(addr) ((addr >> PMD_OFFSET) & PT_ENTRY_MASK)
#define PUD_ENTRY(addr) ((addr >> PUD_OFFSET) & PT_ENTRY_MASK)
#define PGD_ENTRY(addr) ((addr >> PGD_OFFSET) & PT_ENTRY_MASK)


#ifndef ASM_FILE // C code

#include <stdint.h>

#define PAGE_ALIGN(addr)    ((((size_t) addr) + (PAGE_SIZE - 1)) & PAGE_MASK)

#endif

#endif
