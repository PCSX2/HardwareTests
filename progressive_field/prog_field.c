#include <kernel.h>
#include <stdio.h>
#include <graph.h>
#include <draw.h>
#include <gs_psm.h>

#define INTC_STAT (*(volatile u32 *)0x1000F000)
#define GSCSR (*(volatile u64 *)0x12001000)

void setup_gs()
{
	framebuffer_t fb;
	fb.address = graph_vram_allocate(640, 448, GS_PSM_24, GRAPH_ALIGN_PAGE);
	fb.psm = GS_PSM_24;
	fb.width = 640;
	fb.height = 480;

	graph_initialize(fb.address, fb.width, fb.height, fb.psm, 0, 0);
	return;
}

int main(void)
{
	setup_gs();

	INTC_STAT = 0xC;
	while (!(INTC_STAT & 0x8))
	{
	}

	INTC_STAT = 0xC;
	while (!(INTC_STAT & 0x8))
	{
	}
	// Set the mode to progressive
	graph_set_mode(GRAPH_MODE_NONINTERLACED, GRAPH_MODE_NTSC, 0, 0);

	// Clear V-blank start and V-blank end interrupts
	INTC_STAT = 0xC;
	// Wait for V-blank end interrupt
	while (!(INTC_STAT & 0x8))
	{
	}

	// Enable the VSync interrupt
	GSCSR = 0x8;

	for(int i = 0; i < 30; i++)
	{
		// Clear V-blank start and V-blank end interrupts
		INTC_STAT = 0xC;

		// Read FIELD
		u32 start_odd = GSCSR & 0x2000;

		// Wait for V-blank end interrupt
		while (!(INTC_STAT & 0x8))
		{
		}

		// Read FIELD again
		u32 end_odd = GSCSR & 0x2000;

		printf("FIELD Prev %x Field New %x\n", start_odd, end_odd);
	}

	SleepThread();
}
