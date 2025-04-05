#define ASM_FILE 1
#include <boot/multiboot2.h>
#include <boot/types.h>

#define GRUB_MULTIBOOT_ARCHITECTURE_I386 (0)
#define MULTIBOOT2_HEADER_LEN (multiboot_header_end - multiboot_header)
#define MULTIBOOT2_HEADER_CHECKSUM \
    -(MULTIBOOT2_HEADER_MAGIC + GRUB_MULTIBOOT_ARCHITECTURE_I386 + MULTIBOOT2_HEADER_LEN)

/* ==================== Multiboot2 Header ==================== */
.section .multiboot_header
.align 8
multiboot_header:
    .long   MULTIBOOT2_HEADER_MAGIC
    .long   GRUB_MULTIBOOT_ARCHITECTURE_I386
    .long   MULTIBOOT2_HEADER_LEN
    .long   MULTIBOOT2_HEADER_CHECKSUM

    /* Entry Address Tag */
    .align 8
    .short MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS
    .short 0
    .long 12
    .long _start

    /* Framebuffer Tag */
    .align 8
    .short MULTIBOOT_HEADER_TAG_FRAMEBUFFER
    .short 0
    .long 20
    .long 1024
    .long 768
    .long 32

    /* End Tag */
    .align 8
    .short MULTIBOOT_HEADER_TAG_END
    .short 0
    .long 8
multiboot_header_end:

/* ==================== 32-bit Boot Code ==================== */
.section .text
.code32
.align 0x1000
.global _start
.extern kernel_main

_start:
    cli
    cmpl $MULTIBOOT2_BOOTLOADER_MAGIC, %eax
    jne .loop

    movl $stack_top, %esp
    movl %eax, %edi               /* Magic -> edi */
    movl %ebx, %esi               /* MBI -> esi */

    /* Disable Paging */
    movl %cr0, %eax
    andl $~CR0_PG, %eax
    movl %eax, %cr0

    /* Setup Boot Page Tables */
    movl $bootPUD, %eax
    orl $(PDE_ATTR_P | PDE_ATTR_RW), %eax
    movl %eax, boot_pgd

    xorl %eax, %eax
    orl $(PDE_ATTR_P | PDE_ATTR_RW | PDE_ATTR_PS), %eax
    movl %eax, bootPUD

    xorl %eax, %eax
    movl %eax, (boot_pgd + 4)
    movl %eax, (bootPUD + 4)

    /* Load Page Table */
    movl $boot_pgd, %eax
    movl %eax, %cr3

    /* Enable PAE/PGE */
    movl %cr4, %eax
    orl $(CR4_PAE | CR4_PGE), %eax
    movl %eax, %cr4

    /* Enable Long Mode */
    movl $0xC0000080, %ecx
    rdmsr
    orl $(1 << 8), %eax
    wrmsr

    /* Enable Paging */
    movl %cr0, %eax
    orl $CR0_PG, %eax
    movl %eax, %cr0

    /* Load GDT */
    lgdt gdt64_ptr

    /* Jump to 64-bit Code */
    ljmp $(1 << 3), $long_mode_start

.loop:
    hlt
    jmp .loop

/* ==================== 64-bit Code ==================== */
.section .text
.code64
long_mode_start:
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %ss
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    movq $stack_top, %rsp

    /* Correct Parameter Passing */
    movq %rdi, %rdi              /* Magic (32-bit -> 64-bit) */
    movq %rsi, %rsi              /* MBI Pointer */
    call kernel_main

    cli
.halt:
    hlt
    jmp .halt

/* ==================== BSS Section ==================== */
.section .bss
.align 4096

.global boot_pgd, bootPUD, stack_bottom, stack_top

/* Page table */
boot_pgd:
    .space 4096
bootPUD:
    .space 4096

/* 16KB stack */
stack_bottom:
    .space 4096 * 4
stack_top:

/* ==================== GDT ==================== */
.section .rodata
.align 4096

.global gdt64, gdt64_ptr
gdt64:
    .quad 0
gdt64_code_segment:
    .quad 0x00209A0000000000    /* Code Segment */
gdt64_data_segment:
    .quad 0x0000920000000000    /* Data Segment */
gdt64_ptr:
    .short . - gdt64 - 1
    .long gdt64
