#include <stddef.h>
#include <stdint.h>

static char fBuffer[50];
static char sBuffer[50];
int flag = 1;

static void outb(uint16_t port, uint8_t value)
{
	asm("outb %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}

static void outl(uint16_t port, uint32_t value)
{
    asm("outl %0,%1" :     : "a"(value), "Nd"(port) : "memory");
}
static uint32_t inl(uint16_t port)
{
    uint32_t value;
    asm volatile("inl %1, %0" : "=a"(value): "Nd"(port): "memory");
    return value;
}

void HC_print8bit(uint8_t val)
{
	outb(0xE9, val);
}

void HC_print32bit(uint32_t val)
{
	outl(0xEA, val);
}

uint32_t HC_numExits()
{
	uint32_t val = inl(0xEB);
	// outl(0xEA, val);
	return val;
}

void HC_printStr(char *str)
{
    outl(0xEC, (uint32_t)(uintptr_t)str);
}

char *HC_numExitsByType()
{
	char *str = (char *)(uintptr_t)inl(0xED);
	char *buffer = (flag == 1) ? fBuffer : sBuffer;
    flag ^= 1;

    int index = 0;
    for (index = 0; str[index] != '\0'; index++)
        buffer[index] = str[index];
    buffer[index] = '\0';

    return buffer;
}

uint32_t HC_gvaToHva(uint32_t gva)
{
	outl(0xEE, gva);
	uint32_t hva = inl(0xEF);
	return hva;
}

void
	__attribute__((noreturn))
	__attribute__((section(".start")))
	_start(void)
{
	const char *p;

	for (p = "Hello 695!\n"; *p; ++p)
		HC_print8bit(*p);

	/*----------Don't modify this section. We will use grading script---------*/
	/*---Your submission will fail the testcases if you modify this section---*/
	HC_print32bit(2048);
	HC_print32bit(4294967295);

	uint32_t num_exits_a, num_exits_b;
	num_exits_a = HC_numExits();

	char *str = "CS695 Assignment 2\n";
	HC_printStr(str);

	num_exits_b = HC_numExits();

	HC_print32bit(num_exits_a);
	HC_print32bit(num_exits_b);

	char *firststr = HC_numExitsByType();
	uint32_t hva;
	hva = HC_gvaToHva(1024);
	HC_print32bit(hva);
	hva = HC_gvaToHva(4294967295);
	HC_print32bit(hva);
	char *secondstr = HC_numExitsByType();

	HC_printStr(firststr);
	HC_printStr(secondstr);
	/*------------------------------------------------------------------------*/

	*(long *)0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a"(42) : "memory");
}
