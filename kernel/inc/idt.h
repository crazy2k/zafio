#ifndef __IDT_H__

#define __IDT_H__

#include "types.h"
#include "mmu.h"
#include "sched.h"

// Campos de descriptores de la IDT

#define IDT_DESC_P ((uint64_t)0x1 << (32 + 15))
#define IDT_DESC_DPL(dpl) ((uint64_t)(__LOW2_BITS__ & (dpl)) << (32 + 13))
#define IDT_DESC_D ((uint64_t)0x1 << (32 + 11))
#define IDT_DESC_INT ((uint64_t)0x6 << (32 + 8))
#define IDT_DESC_TRAP ((uint64_t)0x7 << (32 + 8))
#define IDT_DESC_SEL(segsel) ((__LOW16_BITS__ & (uint64_t)(segsel)) << 16)
#define IDT_DESC_OFFSET(offset) ((uint64_t)(__LOW16_BITS__ & (uint32_t)(offset)) | \
    ((uint64_t)(__HIGH16_BITS__ & (uint32_t)(offset)) << 32))

// Codigos de error y datos varios

#define IDT_BAD_INDEX   0x1
#define IDT_BUSY        0x2

// Recordar que puede hacer falta actualizar estos valores en idt_handlers.S si
// son alterados aqui
#define IDT_LENGTH      256
#define IDT_LAST_INDEX  (IDT_LENGTH - 1)

// Indices de interrupciones y excepciones
#define IDT_INDEX_GP    13ul
#define IDT_INDEX_PF    14ul
#define IDT_INDEX_TIMER 32ul
#define IDT_INDEX_KB    33ul
#define IDT_INDEX_SYSC  0x80ul

// Constantes de los PIC y sus ICWs

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

// Initialization Command Words para los PICs
#define ICW1_ICW4       0x01    // ICW4 se va a enviar (o no)
#define ICW1_SINGLE     0x02    // Un unico 8259A (o mas de uno)
#define ICW1_INTERVAL4  0x04    // Interrupt vectors de 4 bytes (o de 8)
#define ICW1_LEVEL      0x08    // Interrupciones por nivel (o por flanco)
#define ICW1_INIT       0x10    // Inicializacion

#define ICW3_MATTACH2   0x04    // El slave ingresa por IRQ2
#define ICW3_SATTACH2   0x02    // Se ingresa al master por IRQ2

#define ICW4_8086       0x01    // Modo 8086 (u 8085)
#define ICW4_AUTO       0x02    // EOI automatico (o normal)
#define ICW4_MASTER     0x04    // Master (o esclavo)
#define ICW4_BUFFERED   0x08    // Buffered mode (o nonbuffered)
#define ICW4_SFNM       0x10    // Fully nested (o not fully nested)

#define OCW2            0x20    // EOI (end-of-interrupt)

#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28

#define PIC_TIMER       0x1
#define PIC_KB          0x2

#define PIC_ALL_ENABLED ((~PIC_TIMER) & (~PIC_KB))

// Funciones

typedef void (*isr_t)(uint32_t index, uint32_t error_code, task_state_t *st);

void idt_init();

int register_isr(uint32_t index, isr_t isr);

void idt_handle(uint32_t index, uint32_t error_code, task_state_t *st);
int set_handler(uint32_t index, void (*handler)());

typedef gdtr_t idtr_t;

extern idtr_t idtr;

extern uint64_t idt[IDT_LENGTH];

extern void set_handlers();

#endif
