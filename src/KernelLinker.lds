ENTRY(_start)

PHDRS {
    boot    PT_LOAD FLAGS((1 << 0) | (1 << 2));  /* RX */
    text    PT_LOAD FLAGS((1 << 0) | (1 << 2)); /* RX */
    rodata  PT_LOAD FLAGS((1 << 0));             /* R */
    data    PT_LOAD FLAGS((1 << 0) | (1 << 1)); /* RW */
    bss     PT_LOAD FLAGS((1 << 0) | (1 << 1)); /* RW */
}

SECTIONS {
    . = 0x100000;

    .boot : ALIGN(8) {
        *(.boot.loader.header)
    } :boot

    .text : ALIGN(4K) {
        *(.text .text.*)
        *(.boot.loader.text32)
    } :text

    .rodata : ALIGN(4K) {
        *(.rodata .rodata.*)
    } :rodata

    .data : ALIGN(4K) {
        *(.data .data.*)
        *(.boot.loader.data)
    } :data

    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss .bss.*)
    } :bss

    PROVIDE(_kernel_start = .);

    /DISCARD/ : {
        *(.note.*)
        *(.comment)
    }
}
