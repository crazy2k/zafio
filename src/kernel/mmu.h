#define __LOW16_BITS__ 0x0000FFFF
#define __16_23_BITS__ 0x00FF0000
#define __24_31_BITS__ 0xFF000000
#define __16_19_BITS__ 0x000F0000

// Segmentacion

// GDT addreses

// Base
#define GDT_BASE_ADDR(dir) ( ((__UINT64_TYPE__)(dir) & __LOW16_BITS__) << 16 | \
	((__UINT64_TYPE__)(dir) & HIGH __16_23_BITS__) << 16 | \
	((__UINT64_TYPE__)(dir) & HIGH __24_31_BITS__) << 32 )

// Limit (Se interpreta segun el valor del bit G. Tener en cuenta que
//       representa el tamanio del segmento menos 1.)	
#define GDT_LIMIT(dir) (((__UINT64_TYPE__)(dir) & __LOW16_BITS__) | \
	((__UINT64_TYPE__)(dir) & HIGH __16_19_BITS__) << 40)


//GDT options

// G     (el limite se interpreta en unidades de 1B (0) o 4KB (1))
#define GDT_GRANULARITY_LIMIT_4KB (((__UINT64_TYPE__) 1) << 55)

// D/B   (segmentos de 16 bits (0) o 32 bits (1))
#define GDT_32_BITS_SEGMENT (((__UINT64_TYPE__) 1) << 54)

// L     (codigo que no es de 64 bits (0) o que es de 64 bits (1))
#define GDT_64_BITS_CODE (((__UINT64_TYPE__) 1) << 53)

// AVL   (no dice nada; es un bit disponible para usar, si se quiere)

// P     (segmento ausente en memoria (0) o presente en memoria (1))
#define GDT_PRESENT_SEGMENT (((__UINT64_TYPE__) 1) << 47)

// DPL   (nivel de privilegio del segmento; de 0 a 3)
#define GDT_PRIVILEGE_LEVEL(level) (((__UINT64_TYPE__) level) << 45)

// S     (segmento del sistema (0) o de codigo/datos (1))
#define GDT_APPLICATION_SEGMENT (((__UINT64_TYPE__) 1) << 44)


//GDT type fields
#define GDT_TYPE(type) ((__UINT64_TYPE__)(type) << 40)
	
// Application segment type bits
#define STA_X           0x8         // Executable segment
#define STA_E           0x4         // Expand down (non-executable segments)
#define STA_C           0x4         // Conforming code segment (executable only)
#define STA_W           0x2         // Writeable (non-executable segments)
#define STA_R           0x2         // Readable (executable segments)
#define STA_A           0x1         // Accessed

// System segment type bits
#define STS_T16A        0x1         // Available 16-bit TSS
#define STS_LDT         0x2         // Local Descriptor Table
#define STS_T16B        0x3         // Busy 16-bit TSS
#define STS_CG16        0x4         // 16-bit Call Gate
#define STS_TG          0x5         // Task Gate / Coum Transmitions
#define STS_IG16        0x6         // 16-bit Interrupt Gate
#define STS_TG16        0x7         // 16-bit Trap Gate
#define STS_T32A        0x9         // Available 32-bit TSS
#define STS_T32B        0xB         // Busy 32-bit TSS
#define STS_CG32        0xC         // 32-bit Call Gate
#define STS_IG32        0xE         // 32-bit Interrupt Gate
#define STS_TG32        0xF         // 32-bit Trap Gate

#define GDT_NULL_SEGMENT ((__UINT64_TYPE__) 0)

/* Ejemplo

uint64 segment = GDT_BASE_ADDR(0) | GDT_LIMIT(0xFFFFF) | GDT_TYPE(STA_W) | 
	GDT_PRIVILEGE_LEVEL(0) | GDT_APPLICATION_SEGMENT | GDT_32_BITS_SEGMENT |
	GDT_GRANULARITY_LIMIT_4KB | GDT_PRESENT_SEGMENT;

*/


// Paginación
