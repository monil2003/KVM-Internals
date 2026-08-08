#include <stddef.h>
#include <stdint.h>

static void outl(uint16_t port, uint32_t value){
    asm("outl %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}

void
	__attribute__((noreturn))
	__attribute__((section(".start")))
	_start(void)
{
	uint32_t array[5];

    outl(0x11, (uint32_t)(uintptr_t)array);
    for (;;)
    {
        for (int i = 0; i < 5; i++)
        {
            // Can do something with the array here if needed
        }
        outl(0x12, (uint32_t)(uintptr_t)array);
    }
	*(long *)0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a"(42) : "memory");
}
