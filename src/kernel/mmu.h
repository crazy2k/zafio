#define __LOW16_BITS__ 0x0000FFFF
#define __16_23_BITS__ 0x00FF0000
#define __24_31_BITS__ 0xFF000000
#define __16_19_BITS__ 0x000F0000
#define __12_31_BITS__ 0xFFFFF000

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

#define GDT_DESC_BASE(dir) ( ((__UINT64_TYPE__)(dir) & __LOW16_BITS__) << 16 | \
	((__UINT64_TYPE__)(dir) & HIGH __16_23_BITS__) << (32 - 16) | \
	((__UINT64_TYPE__)(dir) & HIGH __24_31_BITS__) << 32 )

#define GDT_DESC_LIMIT(dir) ( ((__UINT64_TYPE__)(dir) & __LOW16_BITS__) | \
	((__UINT64_TYPE__)(dir) & HIGH __16_19_BITS__) << 32 )

#define GDT_DESC_G(limit) (((__UINT64_TYPE__) limit) << (32 + 23))
#define GDT_DESC_DB(size) (((__UINT64_TYPE__) size) << (32 + 22))
#define GDT_DESC_L(is_64) (((__UINT64_TYPE__) is64) << (32 + 21))
#define GDT_DESC_AVL(value) (((__UINT64_TYPE__) value) << (32 + 20))
#define GDT_DESC_P(present) (((__UINT64_TYPE__) present) << (32 + 15))
#define GDT_DESC_DPL(level) (((__UINT64_TYPE__) level) << (32 + 13))
#define GDT_DESC_S(type) (((__UINT64_TYPE__) type) << (32 + 12))
#define GDT_DESC_TYPE(type) ((__UINT64_TYPE__)(type) << (32 + 8))

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

#define GDT_NULL ((__UINT64_TYPE__) 0)

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


// Page Directory Entry

#define PDE_PT_BASE(addr) ((__UINT32_TYPE__)(addr) & __12_31_BITS__)
#define PDE_AVL(value) ((__UINT32_TYPE__)(value) << 8)
#define PDE_PS(size) (((__UINT32_TYPE__) size) << 7)
#define PDE_AVL(value) (((__UINT32_TYPE__) value) << 6)
#define PDE_A(accessed) (((__UINT32_TYPE__) accessed) << 5)
#define PDE_PCD(cacheable) (((__UINT32_TYPE__) cacheable) << 4)
#define PDE_PWT(wt) (((__UINT32_TYPE__) wt) << 3)
#define PDE_US(level) (((__UINT32_TYPE__) level) << 2)
#define PDE_RW(writable) (((__UINT32_TYPE__) writable) << 1)
#define PDE_P(present) (((__UINT32_TYPE__) present) << 0)

// Page Table Entry

#define PTE_PAGE_BASE(dir) ((__UINT32_TYPE__)(dir) & __12_31_BITS__)
#define PDE_AVL(value) ((__UINT32_TYPE__)(value) << 9)
#define PTE_G(global) (((__UINT32_TYPE__) global) << 8)
#define PTE_PAT(value) (((__UINT32_TYPE__) value) << 7)
#define PTE_D(value) ((__UINT32_TYPE__)(value) << 6)
#define PTE_A(accessed) (PDE_A(accessed))
#define PTE_PCD(cacheable) (PDE_PCD(cacheable)
#define PTE_PWT(wt) (PDE_PWT(wt))
#define PTE_US(level) (PDE_US(level))
#define PTE_RW(writable) (PDE_RW(writable))
#define PTE_P(present) (PDE_P(present))

