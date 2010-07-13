#include "mmu.h"

uint32_t pd[1024] __attribute__((section (".pd"))) = {
    [PDI(KERNEL_PHYS_ADDR)] = PDE_PT_BASE(0) | PTE_PWT |
        PTE_RW | PTE_P,
        
    [PDI(KERNEL_VIRT_ADDR)] = PDE_PT_BASE(0) | PTE_PWT |
        PTE_RW | PTE_P,
};

uint32_t pt_temp_im[1024] = {
    [PTI(KERNEL_PHYS_ADDR)] = PTE_PAGE_BASE(KERNEL_PHYS_ADDR) | PTE_G |
        PTE_PWT | PTE_RW | PTE_P
};



#define COMMON_FLAGS (GDT_DESC_G | GDT_DESC_DB | GDT_DESC_P | GDT_DESC_S)

uint64_t gdt[] __attribute__((section (".gdt"))) = {
    
    /*Null Segment*/ GDT_NULL,
    
    /*kernel_cs*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x0) | GDT_DESC_TYPE(GDT_F_CODE_R) | COMMON_FLAGS,
    
    /*kernel_ds*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x0) | GDT_DESC_TYPE(GDT_F_DATA_W) | COMMON_FLAGS,
    
    /*user_cs*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x3) | GDT_DESC_TYPE(GDT_F_CODE_R) | COMMON_FLAGS,
    
    /*user_ds*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x3) | GDT_DESC_TYPE(GDT_F_DATA_W) | COMMON_FLAGS };



