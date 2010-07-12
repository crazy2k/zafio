#include "mmu.h"

#define COMMON_FLAGS (GDT_DESC_G(0x1) | GDT_DESC_DB(0x1) | GDT_DESC_L(0x0) | \
    GDT_DESC_P(0x1) | GDT_DESC_S(0x1))

uint64_t gdt[] __attribute__((__aligned__(16))) = { 
    
    /*Null Segment*/ GDT_NULL,
    
    /*kernel_cs*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFFFFF) |
    GDT_DESC_DPL(0x0) | GDT_DESC_TYPE(GDT_F_CODE_R) | COMMON_FLAGS,
    
    /*kernel_ds*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFFFFF) |
    GDT_DESC_DPL(0x0) | GDT_DESC_TYPE(GDT_F_DATA_W) | COMMON_FLAGS,
    
    /*user_cs*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFFFFF) |
    GDT_DESC_DPL(0x3) | GDT_DESC_TYPE(GDT_F_CODE_R) | COMMON_FLAGS,
    
    /*user_ds*/ GDT_DESC_BASE(0x0) | GDT_DESC_LIMIT(0xFFFFFFFF) |
    GDT_DESC_DPL(0x3) | GDT_DESC_TYPE(GDT_F_DATA_W) | COMMON_FLAGS };



