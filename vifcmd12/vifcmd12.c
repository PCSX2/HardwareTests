#include <stdio.h>
#include <kernel.h>

// VIF commands
#define VIFNOP 0x00000000
#define VIFMPG(size, pos) ((0x4A000000) | (size << 16) | pos)
#define VIFFLUSHE 0x10000000
#define VIFFLUSH 0x11000000
#define VIFFLUSH12 0x12000000
#define VIFFLUSHA 0x13000000
#define VIFMSCAL(execaddr) (0x14000000 | execaddr)

// VIF1 DMA registers
#define VIF1CHCR (*(volatile u32 *)0x10009000)
#define VIF1MADR (*(volatile u32 *)0x10009010)
#define VIF1QWC (*(volatile u32 *)0x10009020)
#define VIF1STAT (*(volatile u32 *)0x10003c00)

// The micro program gets linked into the .vudata section
extern u64 loop_start __attribute__((section(".vudata")));
extern u64 loop_end __attribute__((section(".vudata")));

// Uploads a micro program to the VU1
// offset holds where the micro program is stored in the VU1 code memory
// start and end point to the start and end of the micro program
// flushwith is the last instruction of the _vifcode_
// Usually you would use VIFFLUSH(10h) or VIFFLUSHE(11h) or VIFFLUSHA(13h)
// For this experiement we use (12h)

void uploadMicroProgram(const u32 offset, const u64 *start, const u64 *end, u32 flushwith)
{
	u32 vp[0xFFF] __attribute__((aligned(128)));
	u32 vpi = 0;

	u32 progInstructions = (end - start);

	u32 mpgBlocks = 0;
	vp[vpi++] = VIFFLUSHE;
	for (u32 instruction = 0; instruction < progInstructions; instruction++)
	{
		if (!(instruction % 256))
		{
			u32 size = progInstructions - (mpgBlocks * 256); // Get the remaining amount of instructions to send
			if (size >= 256)								 // If the remaining instructions to be sent to VIF is >= 256
				size = 0;									 // Then send the maximum amount in this mpg (0, but is interpreted as 256)

			// If we are in our first mpg block then set the offset to be zero
			vp[vpi++] = VIFMPG((size + offset), (mpgBlocks == 0 ? 0 : (mpgBlocks * 256)));

			mpgBlocks++;
		}
		vp[vpi++] = start[instruction];
		vp[vpi++] = start[instruction] >> 32;
	}

	vp[vpi++] = VIFMSCAL(0); // Set the microprogram to be executed
	vp[vpi++] = flushwith;

	while (vpi % 4) // Align the packet
		vp[vpi++] = VIFNOP;

	VIF1MADR = (u32)&vp[0];
	VIF1QWC = vpi / 4;

	FlushCache(0);

	VIF1CHCR = 0x101;

	FlushCache(0);

	while (VIF1CHCR == 0x100)
	{
	}
	return;
}

int main()
{
	printf("VIF1STAT ER1 = %x\n", VIF1STAT & 0x2000);

	// Upload a micro program to the VU1
	// Replace VIFFLUSH12 with VIFFLUSHE to wait for an E bit
	uploadMicroProgram(0, &loop_start, &loop_end, VIFFLUSH12);

	u32 cnt = 0;
	u32 cnt2 = 0;

	// Loop if the VIF is:
	// Not idle (bit 0)
	// E bit waiting (bit 1)
	// Waiting for a GIF transfer end (bit 2)
	while ((VIF1STAT & 0x7))
	{
		if (cnt == 10000000)
		{
			cnt2++;
			printf("waited %d mil times (VIF1STAT ER1 = %x)\n", cnt2 * 10, VIF1STAT & 8192);
			cnt = 0;
		}

		cnt++;
	}
	printf("VIF1 finished, VIF1STAT ER1 = %x\n", VIF1STAT & 0x2000);

	SleepThread();
}
