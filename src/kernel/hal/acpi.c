#include <boot/multiboot2.h>
#include <kernel/hal/acpi.h>
#include <kernel/stdio.h>

// 全局变量，存储 FADT 的地址
static const struct FADT *fadt = NULL;
static const struct ACPISDTHeader *ssdt = NULL;
static const struct ACPISDTHeader *dsdt = NULL;

// 计算校验和的函数
static bool acpi_checksum_valid(void *table, uint32_t length) {
    uint8_t *ptr = (uint8_t *)table;
    uint8_t sum = 0;
    for (uint32_t i = 0; i < length; i++) {
        sum += ptr[i];
    }
    return sum == 0;
}

// static int acpi_enable(const struct ACPISDTHeader *_fadt, const struct ACPISDTHeader *_ssdt) {
//     if(ssdt == NULL) {
//         ssdt = _ssdt;
//     }
//     if(fadt != NULL) { // FADT has been initialized.
//         return 0;
//     }
//     if(_fadt == NULL) { // FADT is not initialized yet, but there is no FADT in param.
//         return -1;
//     }
//     fadt = (const struct ACPISDTHeader *)_fadt, ssdt = 
// }

// 解析 RSDP
static void *find_rsdp(multiboot_uint8_t *multiboot_header) {
    struct multiboot_tag *tag = (struct multiboot_tag *)(multiboot_header + 8);
    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        if (tag->type == MULTIBOOT_TAG_TYPE_ACPI_NEW) {
            return (void *)(tag + 1);
        }
        tag = (struct multiboot_tag *)(((multiboot_uint8_t *)(tag)) + ((tag->size + 7) & ~7));
    }
    return NULL;
}

// 解析 DT(RSDT or XSDT)
static void *find_rsdt_or_xsdt(void *rsdp) {
    struct XSDP_t *xsdp = (struct XSDP_t *)rsdp;
    if (xsdp->Revision >= 2) {
        // 使用 XSDT
        return (void *)(xsdp->XsdtAddress);
    } else {
        // 使用 RSDT
        return (void *)((uint64_t)xsdp->RsdtAddress);
    }
}

// 解析DT并初始化各种乱七八糟的表
static void *find_fadt(void *rsdt_or_xsdt) {
    struct RSDT *rsdt = (struct RSDT *)rsdt_or_xsdt;
    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;

    for(int i = 0; i < entries; ++i) {
        struct ACPISDTHeader *h = (struct ACPISDTHeader *)(uint64_t)rsdt->PointerToOtherSDT[i];
        if(!strncmp(h->Signature, "FACP", 4))
            return (void *)h;
    }
    return NULL;
}

// 初始化 ACPI
int acpi_init(multiboot_uint8_t *multiboot_header) {
    // 查找 RSDP
    void *rsdp = find_rsdp(multiboot_header);
    if (!rsdp) {
        return -1;
    }

    // 验证 RSDP 校验和
    if (!acpi_checksum_valid(rsdp, ((struct XSDP_t *)rsdp)->Length)) {
        return -2;
    }

    // 查找 RSDT 或 XSDT
    void *rsdt_or_xsdt = find_rsdt_or_xsdt(rsdp);
    if (!rsdt_or_xsdt) {
        return -3;
    }

    // 验证 RSDT/XSDT 校验和
    if (!acpi_checksum_valid(rsdt_or_xsdt, ((struct ACPISDTHeader *)rsdt_or_xsdt)->Length)) {
        return -4;
    }

    // 查找 FADT
    fadt = (struct FADT *)find_fadt(rsdt_or_xsdt);
    if (!fadt) {
        return -5;
    }

    // 验证 FADT 校验和
    if (!acpi_checksum_valid((void *)fadt, fadt->h.Length)) {
        return -6;
    }

    return 0;
}

// 关机
void poweroff() {
    if (!fadt) {
        return;
    }
    asm volatile ("cli");

    // 获取 PM1a Control Block 地址
    uint16_t pm1a_cnt = fadt->PM1aControlBlock;

    // 设置 SLP_TYPa 和 SLP_EN
    outw(pm1a_cnt, 0x1); // 假设 SLP_TYPa 为 1

    // 等待关机
    while (1);
}

// 重启
void reboot() {
    if (!fadt) {
        return;
    }

    // 获取 Reset Register 地址
    struct GenericAddressStructure *reset_reg = (struct GenericAddressStructure *)&fadt->ResetReg;

    CLINT();

    // 写入 Reset Value
    if (reset_reg->AddressSpace == 1) { // System I/O
        outb(reset_reg->Address, fadt->ResetValue);
    }
}
