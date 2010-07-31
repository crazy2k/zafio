#include "../inc/mmu.h"
#include "../inc/memlayout.h"

uint32_t kernel_pd[1024] __attribute__((section (".pd"))) = {0};

#define COMMON_FLAGS (GDT_DESC_G | GDT_DESC_DB | GDT_DESC_P | GDT_DESC_S)

uint64_t gdt[] __attribute__((aligned(0x08))) = {

    [GDT_INDEX_NULL] = GDT_NULL,

    [GDT_INDEX_KERNEL_CS] = GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x0) | GDT_DESC_TYPE(GDT_F_CODE_R) | COMMON_FLAGS,

    [GDT_INDEX_KERNEL_DS] = GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x0) | GDT_DESC_TYPE(GDT_F_DATA_W) | COMMON_FLAGS,

    [GDT_INDEX_USER_CS] = GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x3) | GDT_DESC_TYPE(GDT_F_CODE_R) | COMMON_FLAGS,

    [GDT_INDEX_USER_DS] = GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x3) | GDT_DESC_TYPE(GDT_F_DATA_W) | COMMON_FLAGS 
};

gdtr_t gdtr = { .size = sizeof(gdt) - 1, .addr = KPHADDR(gdt) }; 


