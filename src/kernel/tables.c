#include "../inc/mmu.h"
#include "../inc/memlayout.h"

uint32_t kernel_pd[1024] __attribute__((section (".pd"))) = {
    [PDI(KERNEL_PHYS_ADDR)] = PDE_PT_BASE(KPHADDR(KERNEL_PAGE_TABLE)) |
        PDE_PWT | PDE_RW| PDE_P,
        
    [PDI(KERNEL_VIRT_ADDR)] = PDE_PT_BASE(KPHADDR(KERNEL_PAGE_TABLE)) |
        PDE_PWT | PDE_RW | PDE_P,
};

uint32_t kernel_pt[1024] __attribute__((section (".pt"))) = {
    [PTI(VIDEO_PHYS_ADDR)] = PTE_PAGE_BASE(VIDEO_PHYS_ADDR) | PTE_G |
        PTE_PWT | PTE_RW | PTE_P,

    [PTI(LOADER_PHYS_ADDR)] = PTE_PAGE_BASE(LOADER_PHYS_ADDR) | PTE_G |
        PTE_PWT | PTE_RW | PTE_P,
        
    // Usamos dos paginas para codigo y datos del kernel
    [PTI(KERNEL_VIRT_ADDR)] = PTE_PAGE_BASE(KERNEL_PHYS_ADDR) | PTE_G |
        PTE_PWT | PTE_RW | PTE_P,

    [PTI(KERNEL_VIRT_ADDR + PAGE_SIZE)] =
        PTE_PAGE_BASE(KERNEL_PHYS_ADDR + PAGE_SIZE) | PTE_G | PTE_PWT |
        PTE_RW | PTE_P,


    // Con una pagina para el stack alcanza (y sobra) al inicio
    [PTI(KERNEL_STACK)] = PTE_PAGE_BASE(KPHADDR(KERNEL_STACK)) | PTE_G |
        PTE_PWT | PTE_RW | PTE_P,

    [PTI(KERNEL_PAGE_TABLE)] = PTE_PAGE_BASE(KPHADDR(KERNEL_PAGE_TABLE)) | 
        PTE_G | PTE_PWT | PTE_RW | PTE_P,

    [PTI(KERNEL_PAGE_DIRECTORY)] = PTE_PAGE_BASE(KPHADDR(KERNEL_PAGE_DIRECTORY)) | 
        PTE_G | PTE_PWT | PTE_RW | PTE_P,

};



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

struct { 
    uint16_t size __attribute__((packed));
    void *addr __attribute__((packed));
} gdtr = { sizeof(gdt) - 1, KPHADDR(gdt) }; 


