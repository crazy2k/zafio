#include "../inc/mmu.h"
#include "../inc/memlayout.h"

uint32_t kernel_pd[1024] __attribute__((section (".pd"))) = {
    [PDI(KERNEL_PHYS_ADDR)] = PDE_PT_BASE(KPHADDR(KERNEL_PAGE_TABLE)) |
        PTE_PWT | PTE_RW | PTE_P,
        
    [PDI(KERNEL_VIRT_ADDR)] = PDE_PT_BASE(KPHADDR(KERNEL_PAGE_TABLE)) |
        PTE_PWT | PTE_RW | PTE_P,
};

uint32_t kernel_pt[1024] __attribute__((section (".pt"))) = {
    [PTI(VIDEO_MEMORY)] = PTE_PAGE_BASE(VIDEO_MEMORY) | PTE_G |
        PTE_PWT | PTE_RW | PTE_P,

    [PTI(LOADER_ADDR)] = PTE_PAGE_BASE(LOADER_ADDR) | PTE_G |
        PTE_PWT | PTE_RW | PTE_P,
        
    [PTI(KERNEL_VIRT_ADDR)] = PTE_PAGE_BASE(KERNEL_PHYS_ADDR) | PTE_G |
        PTE_PWT | PTE_RW | PTE_P,
        
    [PTI(KERNEL_PAGE_TABLE)] = PTE_PAGE_BASE(KPHADDR(KERNEL_PAGE_TABLE)) | 
        PTE_G | PTE_PWT | PTE_RW | PTE_P,

    [PTI(KERNEL_PAGE_DIRECTORY)] = PTE_PAGE_BASE(KPHADDR(KERNEL_PAGE_DIRECTORY)) | 
        PTE_G | PTE_PWT | PTE_RW | PTE_P,
        
    [PTI(KERNEL_STACK)] = PTE_PAGE_BASE(KPHADDR(KERNEL_STACK)) | PTE_G |
        PTE_PWT | PTE_RW | PTE_P,
};



#define COMMON_FLAGS (GDT_DESC_G | GDT_DESC_DB | GDT_DESC_P | GDT_DESC_S)

uint64_t gdt[] __attribute__((aligned(0x08))) = {
    
    /*Null Segment*/ GDT_NULL,
    
    /*kernel_cs*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x0) | GDT_DESC_TYPE(GDT_F_CODE_R) | COMMON_FLAGS,
    
    /*kernel_ds*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x0) | GDT_DESC_TYPE(GDT_F_DATA_W) | COMMON_FLAGS,
    
    /*user_cs*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x3) | GDT_DESC_TYPE(GDT_F_CODE_R) | COMMON_FLAGS,
    
    /*user_ds*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFF) |
    GDT_DESC_DPL(0x3) | GDT_DESC_TYPE(GDT_F_DATA_W) | COMMON_FLAGS 
};

struct { 
    uint16_t size __attribute__((packed));
    uint32_t dir  __attribute__((packed));
} gdtr __attribute__((aligned(0x08))) = { sizeof(gdt) - 1, KPHADDR(gdt) }; 


