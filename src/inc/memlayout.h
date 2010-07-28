#ifndef __MEMLAYOUT_H__

#define __MEMLAYOUT_H__

#include <stdint.h>
#include "types.h"

#define KERNEL_OFFSET 0xC0000000
#define KPHADDR(dir) ((void*) (((uint32_t) (dir)) - KERNEL_OFFSET))
#define KVIRTADDR(dir) ((void*) (((uint32_t) (dir)) + KERNEL_OFFSET))

#define KERNEL_VIRT_ADDR 0xC0200000
#define KERNEL_PHYS_ADDR KPHADDR(KERNEL_VIRT_ADDR)

#define KERNEL_STACK (KERNEL_VIRT_ADDR - PAGE_SIZE*254)
#define KERNEL_PAGE_TABLE (KERNEL_VIRT_ADDR - PAGE_SIZE*253)
#define KERNEL_PAGE_DIRECTORY (KERNEL_VIRT_ADDR - PAGE_SIZE*252)

#define LOADER_PHYS_ADDR 0x00100000

#define VIDEO_VIRT_ADDR 0xC00B8000
#define VIDEO_PHYS_ADDR KPHADDR(VIDEO_VIRT_ADDR)

/*

Mapa de memoria física
======================

+----------------------------+ 0x00000000 <--- 0MB
| Memoria usada por PCs      |
| viejas                     |
+----------------------------+ 0x000A0000
| VGA                        | 
+----------------------------+ 0x000C0000
| Dispositivos de 16bits,    |
| etc.                       |
+----------------------------+ 0x000F0000
| BIOS ROM (¿mapeado?)       |
+----------------------------+ 0x00100000 <--- 1MB
| Header Multiboot;          |
| codigo de loader.S;        |
| mbihelpers;                |
+----------------------------+ 0x00101000
| Stack del kernel           |
+----------------------------+ 0x00103000
| Tabla de paginas del       |
| kernel                     |
+----------------------------+ 0x00104000
| Directorio de paginas del  |
| kernel                     |
+----------------------------+ 0x00105000
| Tablas de paginas para     |
| direccionar 1004MB fisicos |
+----------------------------+ 0x00200000
| 0odigo y datos del kernel  |
+----------------------------+ 0x002XXXXX
| 0omienzo de estructuras    |
| page_t                     |
+----------------------------+ 0x00400000   <--- 4MB
| Más estructuras page_t     |
+----------------------------+ 0x0XXXXXXX
| ...                        |
+----------------------------+
| dispositivos de 32 bits    |
+----------------------------+ 0xFFFFFFFF

Hasta el primer MB (0x00100000), hay partes de la memoria que no se
pueden utilizar. Para evitarnos tener que cargar el kernel de a partes
en distintos lugares de la memoria, optamos por utilizar, para el
kernel, las direcciones desde el comienzo del segundo MB.


Mapa de memoria virtual
=======================

+----------------------------+ 0x00000000   <--- 0MB
| ...                        |             \
+----------------------------+ 0x000B8000   | En el primer MB no va
| Memoria de video           |              | nada del kernel
+----------------------------+ 0x000B9000   |
| ...                        |             /
+----------------------------+ 0x00100000
| Header Multiboot;          |
| codigo de loader.S;        |
| mbihelpers;                |
+----------------------------+ 0x00101000
| Stack del kernel           |
+----------------------------+ 0x00103000
| Tabla de paginas del       |
| kernel                     |
+----------------------------+ 0x00104000
| Directorio de paginas del  |
| kernel                     |
+----------------------------+ 0x00105000
| Tablas de paginas para     |
| direccionar 1004MB fisicos |
+----------------------------+ 0x00200000
| Codigo y datos del kernel  |
+----------------------------+ 0x002XXXXX
| Comienzo de estructuras    |
| page_t                     |
+----------------------------+ 0x00400000   <--- 4MB
| Más estructuras page_t     |
+----------------------------+ 0x0XXXXXXX
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
| ...                        |
+----------------------------+ 0xC0000000   <--- 3GB
| ...                        |             \
+----------------------------+ 0xC00B8000   | En el primer MB no va
| Memoria de video           |              | nada del kernel
+----------------------------+ 0xC00B9000   |
| ...                        |             /
+----------------------------+ 0xC0100000
| Loader                     |
+----------------------------+ 0xC0101000
| Stack del kernel           |
+----------------------------+ 0xC0103000
| Tabla de paginas del       |
| kernel                     |
+----------------------------+ 0xC0104000
| Directorio de paginas del  |
| kernel                     |
+----------------------------+ 0xC0105000
| Tablas de paginas para     |
| direccionar 1004MB fisicos |
+----------------------------+ 0xC0200000
| Codigo y datos del kernel  |
+----------------------------+ 0xC02XXXXX
| Comienzo de estructuras    |
| page_t                     |
+----------------------------+ 0xC0400000   <--- 3GB + 4MB
| Más estructuras page_t     |
+----------------------------+ 0xCXXXXXXX
|           ...              |
|           ...              |
|           ...              |
|           ...              |
|           ...              |
|           ...              |
+----------------------------+ 0xFFFFFFFF
*/

#endif
