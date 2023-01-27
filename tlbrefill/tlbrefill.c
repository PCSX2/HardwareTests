#include <debug.h>
#include <kernel.h>
#include <stdio.h>
#include <sio.h>

// Network based logging does not work for me
// So I'll provide both network and ee-sio logs
void dbgmsg(const char* fmt, ...)
{
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, 256, fmt, ap);
	vprintf(buf,ap);
	va_end(ap);
	sio_puts(buf);
}

void tlbRefillHandler();
void* tlbRefillEPC = 0;
void* tlbRefillBadVAddr = 0;

__attribute__ ((__noinline__))
void * get_pc () { return __builtin_return_address(0); }

int main(void)
{
	dbgmsg("TLB Refill handler test running\n");
	// Disable interrupts, don't risk having our handler called unintentionally
	DIntr();

	// Replace the existing TLB refill handler
	// (3) -> Store
	// (2) -> Load
	void* prevVTLBRefillHandler = GetExceptionHandler(3);
	SetVTLBRefillHandler(3, (void*)tlbRefillHandler);

	// Cause a TLB store miss
	asm("nop":::"memory");
	volatile u8* badAddress = (volatile u8*)0x123;
	void* exceptionPC = get_pc() + 8;
	*badAddress = ~1;
	asm("nop":::"memory");

	// Restore the previous TLB refill handler
	SetVTLBRefillHandler(3, prevVTLBRefillHandler);

	EIntr();

	dbgmsg(
		"TLB handler test completed\n"
		" Expected EPC:    +-%p\n"
		" Returned EPC:      %p\n"
		" Expected BadVAddr: %p\n"
		" Returned BadVAddr: %p\n",
		exceptionPC, tlbRefillEPC,
		(void*)badAddress, tlbRefillBadVAddr
		);

	dbgmsg("TLB Refill handler test completed\n");
	SleepThread();
}
