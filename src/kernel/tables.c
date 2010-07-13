#include "mmu.h"

uint64_t pt_temp_im[128] __attribute__((__aligned__(4096))) = {
    [PTI(KERNEL_PHYS_ADDR)] = PTE_PAGE_BASE(KERNEL_PHYS_ADDR) | PTE_G(0x1) |
        PTE_PAT(0x0) | PTE_D(0x0) | PTE_A(0x0) | PTE_PCD(0x0) | PTE_PWT(0x1) |
        PTE_US(0x0) | PTE_RW(0x1) | PTE_P(0x1)
};


uint64_t pd[128] __attribute__((__aligned__(4096))) = {
    [PDI(KERNEL_PHYS_ADDR)] = PDE_PT_BASE(&pt_temp_im) | PDE_PS(0x0) |
        PDE_A(0x0) | PDE_PCD(0x0) | PDE_PWT(0x1) | PDE_US(0x0) | PDE_RW(0x1) |
        PDE_P(0x1),
    [PDI(KERNEL_VIRT_ADDR)] = PDE_PT_BASE(&pt_temp_im) | PDE_PS(0x0) |
        PDE_A(0x0) | PDE_PCD(0x0) | PDE_PWT(0x1) | PDE_US(0x0) | PDE_RW(0x1) |
        PDE_P(0x1),
};



#define COMMON_FLAGS (GDT_DESC_G(0x1) | GDT_DESC_DB(0x1) | GDT_DESC_L(0x0) | \
    GDT_DESC_P(0x1) | GDT_DESC_S(0x1))

uint64_t gdt[] __attribute__((__aligned__(16))) = { 
    
    /*Null Segment*/ GDT_NULL,
    
    /*kernel_cs*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x0) | GDT_DESC_TYPE(GDT_F_CODE_R) | COMMON_FLAGS,
    
    /*kernel_ds*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x0) | GDT_DESC_TYPE(GDT_F_DATA_W) | COMMON_FLAGS,
    
    /*user_cs*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x3) | GDT_DESC_TYPE(GDT_F_CODE_R) | COMMON_FLAGS,
    
    /*user_ds*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x3) | GDT_DESC_TYPE(GDT_F_DATA_W) | COMMON_FLAGS };



