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

/* Ejemplo

uint64 segment = GDT_BASE_ADDR(0) | GDT_LIMIT(0xFFFFF) | GDT_TYPE(STA_W) | 
	GDT_PRIVILEGE_LEVEL(0) | GDT_APPLICATION_SEGMENT | GDT_32_BITS_SEGMENT |
	GDT_GRANULARITY_LIMIT_4KB | GDT_PRESENT_SEGMENT;

*/
/*----------------------------------------------------------------------------*/


// Paginación

// Page Directory Entry

// PT Base   (direccion fisica de la tabla de paginas)
#define PDE_BASE_ADDR(dir) ((__UINT32_TYPE__)(dir) & __12_31_BITS__)

// AVL       (no dice nada// esta disponible para usar, si se quiere)

// G         (indica si la pagina es global (1) o si no lo es (0))
#define PDE_GLOBAL_PAGE (((__UINT32_TYPE__) 1) << 9)

// PS        (indica si las paginas son de 4KB (0) o 4MB (1))
#define PDE_PAGE_SIZE_4MB (((__UINT32_TYPE__) 1) << 8)

// A         (indica si la pagina fue accedida (1) o no (0))
#define PDE_ACCESSED (((__UINT32_TYPE__) 1) << 6)

// PCD       (indica si la pagina/tabla de paginas puede ser
//           cacheada (0) o no (1))
#define PDE_CACHEABLE_PAGE (((__UINT32_TYPE__) 1) << 5)

// PWT       (indica si se realizara write-through (1) o
//           write-back (0) de la pagina/tabla de paginas)
#define PDE_WRITE_THROUGH (((__UINT32_TYPE__) 1) << 4)

// U/S       (indica si el nivel de privilegios asignado a la pagina
//           es de supervisor (0) o de usuario (1)
#define PDE_USER_PAGE (((__UINT32_TYPE__) 1) << 3)

// R/W       (indica si la pagina es de solo lectura (0) o puede
//           leerse y escribirse (1))
#define PDE_WRITEABLE (((__UINT32_TYPE__) 1) << 2)

// P         (indica si la pagina/tabla de paginas esta presente (1)
//           en memoria o no (0))
#define PDE_PRESENT (((__UINT32_TYPE__) 1) << 1)


// Page Table Entry

// Page Base (direccion fisica de la pagina)
#define PTE_BASE_ADDR(dir) ((__UINT32_TYPE__)(dir) & __12_31_BITS__)

// D         (indica si la pagina fue escrita (1) o no (0))
#define PTE_DIRTY_PAGE (((__UINT32_TYPE__) 1) << 1)


/* Ejemplo

uint32 page_table_entry = PTE_BASE_ADDR(0xC1000000) | PDE_PRESENT | PDE_ACCESSED;

*/
