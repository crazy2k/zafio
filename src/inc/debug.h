// Magic breakpoint para bochs
#define BOCHS_BREAK __asm__ __volatile__("xchg %bx, %bx" )     

#define OUTPORTB(_port, _data) __asm__ __volatile__ ("outb %%al, %0" : : "dN" (_port), "a" (_data))

// Imprime character en la debug console
#define BOCHS_PUTS_CHAR(c) OUTPORTB(0xe9, c)

// Stops simulation and breaks into the debug console
//#define BochsBreak() ({ outportw(0x8A00,0x8A00); outportw(0x8A00,0x08AE0); })

