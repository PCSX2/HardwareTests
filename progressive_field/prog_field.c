#include <kernel.h>
#include <stdio.h>

#define INTC_STAT (*(volatile u32 *)0x1000F000)
#define GSCSR (*(volatile u64 *)0x12001000)
#define SYNCV (*(volatile u64 *)0x12000060)
#define SMODE1 (*(volatile u64 *)0x12000010)

static inline void wait_intc_vsync()
{
	INTC_STAT = 0xC;
	while (!(INTC_STAT & 0x8))
		;
}

void test_gs_field()
{
	for (int i = 0; i <= 9; i++)
	{
		printf("    Checking VFP %d\n", i);

		// Bottom bits of SYNCV are the VFP
		SYNCV = 0xC7800003000000LL + i;

		// Wait for two vsyncs here, otherwise our current_field could be
		// wrong.
		wait_intc_vsync();
		wait_intc_vsync();

		u32 current_field = GSCSR & 0x2000;

		for (u32 j = 0; j < 5; j++)
		{
			wait_intc_vsync();
			if (current_field != (GSCSR & 0x2000))
			{
				goto field_changed;
			}
		}

		printf("        FIELD IS NOT CHANGING\n");
		continue;
	field_changed:
		printf("        FIELD IS CHANGING\n");
	}
}

int main(void)
{
	wait_intc_vsync();

	// Enable VSYNC event
	GSCSR = 0x8;

	printf("Checking FIELD when CMOD is set to 1\n");
	SMODE1 |= (1 << 13);
	test_gs_field();
	printf("Checking FIELD when CMOD is set to 0\n");
	SMODE1 &= ~(2 << 13);
	test_gs_field();

	SleepThread();
}
