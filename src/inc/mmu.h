#ifndef __MMU_H__

#define __MMU_H__

#include "types.h"

#define __LOW2_BITS__ 0x00000003
#define __LOW16_BITS__ 0x0000FFFF
#define __16_23_BITS__ 0x00FF0000
#define __24_31_BITS__ 0xFF000000
#define __16_19_BITS__ 0x000F0000
#define __12_31_BITS__ 0xFFFFF000

#define __22_31_BITS__ 0xFFC00000
#define __12_21_BITS__ 0x003FF000

#define PDI(laddr) (((uint32_t)(laddr) & __22_31_BITS__) >> 22)
#define PTI(laddr) (((uint32_t)(laddr) & __12_21_BITS__) >> 12)

/* Segmentacion
 * ============
 * 
 * Convenciones:
 * - Todas las macros relacionadas con la GDT comienzan con el
 *   prefijo ``GDT_``.
 * - Aquellas relacionadas con descriptores en la GDT, llevan el prefijo
 *   ``GDT_DESC_``. El resto del nombre de la macro lleva el nombre dado en
 *   los manuales de Intel, con la salvedad de que los espacios se reemplazan
 *   por ``_``, el caracter ``/`` se omite y todas las letras van en mayuscula.
 * - Los flags, por comodidad, tienen solo el prefijo ``GDT_F``.
 *   Los flags para tipos de segmentos de aplicaciones siguen con ``_DATA_X``
 *   o ``_CODE_X`` segun sean de datos o de codigo, donde X representa el
 *   nombre que se le da en los manuales a cada bit que compone el tipo.
 *   Los flags para tipos de sistema siguen con las siglas correspondientes al
 *   tipo que representan segun el nombre que se le da en los manuales de
 *   Intel.
 */
#define GDT_DESC_BASE(dir) ( ((uint64_t)(dir) & __LOW16_BITS__) << 16 | \
	((uint64_t)(dir) & __16_23_BITS__) << (32 - 16) | \
	((uint64_t)(dir) & __24_31_BITS__) << 32 )

#define GDT_DESC_LIMIT(dir) ( ((uint64_t)(dir) & __LOW16_BITS__) | \
	((uint64_t)(dir) & __16_19_BITS__) << 32 )

#define GDT_DESC_G (((uint64_t) 1) << (32 + 23))
#define GDT_DESC_DB (((uint64_t) 1) << (32 + 22))
#define GDT_DESC_L (((uint64_t) 1) << (32 + 21))
#define GDT_DESC_P (((uint64_t) 1) << (32 + 15))
#define GDT_DESC_DPL(level) (((uint64_t) level) << (32 + 13))
#define GDT_DESC_S (((uint64_t) 1) << (32 + 12))
#define GDT_DESC_TYPE(type) ((uint64_t)(type) << (32 + 8))

// Flags para tipos de aplicaciones
#define GDT_F_DATA_A    0x1 // accessed
#define GDT_F_DATA_W    0x2 // write-enable
#define GDT_F_DATA_E    0x4 // expansion-direction
#define GDT_F_CODE_A    0x9 // accessed
#define GDT_F_CODE_R    0xA // read enable
#define GDT_F_CODE_C    0xC // conforming

// Flags para tipos de sistema
#define GDT_F_16BTA     0x1 // 16-bit TSS (Available)
#define GDT_F_LDT       0x2 // LDT
#define GDT_F_16BTB     0x3 // 16-bit TSS (Busy)
#define GDT_F_16BCG     0x4 // 16-bite Call Gate
#define GDT_F_TG        0x5 // Task Gate
#define GDT_F_16BIG     0x6 // 16-bit Interrupt Gate
#define GDT_F_16BTG     0x7 // 16-bit Trap Gate
#define GDT_F_32BTA     0x9 // 32-bit TSS (Available)
#define GDT_F_32BTB     0xB // 32-bit TSS (Busy)
#define GDT_F_32BCG     0xC // 32-bit Call Gate
#define GDT_F_32BIG     0xE // 32-bit Interrupt Gate
#define GDT_F_32BTG     0xF // 32-bit Trap Gate

#define GDT_NULL ((uint64_t) 0)

#define GDT_SEGSEL(rpl, index) \
    (((rpl) && __LOW2_BITS__) | ((index) << 3))

/* Paginacion
 * ==========
 * 
 * Convenciones:
 * - Todas las macros relacionadas con entradas en el directorio de paginas
 *   comienzan con ``PDE_``, y las relacionadas con tablas de paginas, con
 *   ``PDE_``. El resto del nombre de la macro lleva el nombre dado en los
 *   manuales de Intel a cada campo, con la salvedad de que los espacios se
 *   reemplazan por ``_``, el caracter ``/`` se omite y todas las letras van
 *   en mayuscula.
 */

// page_t Directory Entry

#define PDE_PT_BASE(addr) ((uint32_t)(addr) & __12_31_BITS__)
#define PDE_PS (((uint32_t) 1) << 7)
#define PDE_A (((uint32_t) 1) << 5)
#define PDE_PCD (((uint32_t) 1) << 4)
#define PDE_PWT (((uint32_t) 1) << 3)
#define PDE_US (((uint32_t) 1) << 2)
#define PDE_RW (((uint32_t) 1) << 1)
#define PDE_P (((uint32_t) 1) << 0)

// page_t Table Entry

#define PTE_PAGE_BASE(dir) ((uint32_t)(dir) & __12_31_BITS__)
#define PTE_G (((uint32_t) 1) << 8)
#define PTE_PAT (((uint32_t) 1) << 7)
#define PTE_D (((uint32_t) 1) << 6)
#define PTE_A PDE_A
#define PTE_PCD PDE_PCD
#define PTE_PWT PDE_PWT
#define PTE_US PDE_US
#define PTE_RW PDE_RW
#define PTE_P PDE_P

typedef struct { 
    uint16_t size __attribute__((packed));
    void *addr __attribute__((packed));
} gdtr_t; 

extern gdtr_t gdtr;

extern uint64_t gdt[];


typedef struct page_t page_t;

struct page_t {
    int count;
    page_t *next;
    page_t *prev;
};

#define PAGE_SIZE 0x1000ul

#define PAGE_4MB_SIZE 0x400000ul

//La mayor cantiad de memoria q muede mapear el kernel
#define MAX_KERNEL_MEMORY \
    (((uint32_t)KERNEL_VIRT_ADDR - (uint32_t)kernel_pts)*1024)

//Ultima direccion virtual q puede utilizar el kernel
#define LAST_KERNEL_VADDR ((void *)(MAX_KERNEL_MEMORY + KERNEL_OFFSET))

#define ALIGN_TO_4MB(addr, ceil) ALIGN_TO_N(addr, 0xFFC00000, 0x400000, ceil)

#define ALIGN_TO_PAGE(addr, ceil) ALIGN_TO_N(addr, 0xFFFFF000, 0x1000, ceil)

#define ALIGN_TO_N(addr, mask, n, ceil) ({ uint32_t __addr = (uint32_t)(addr); \
    uint32_t __mask = (uint32_t)(mask); \
    uint32_t __n = (uint32_t)(n); \
    if ((ceil) && (__addr & ~__mask)) __addr += __n; \
    (void*) (__addr & __mask); })

#endif

