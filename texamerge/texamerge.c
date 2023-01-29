#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <sio.h>

#include <dma.h>
#include <draw.h>
#include <graph.h>
#include <gs_psm.h>
#include <gs_gp.h>
#include <gs_privileged.h>

void dbgmsg(const char *fmt, ...)
{
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, 256, fmt, ap);
	vprintf(buf, ap);
	va_end(ap);
	sio_puts(buf);
}

typedef union
{
	struct
	{
		u16 R : 5;
		u16 G : 5;
		u16 B : 5;
		u16 A : 1;
	};
	u16 _u16;
} RGBA16TEXEL;

// Generic GS environment setup code
framebuffer_t fb;
zbuffer_t zb;
void init_gs(void)
{
	fb.width = 640;
	fb.height = 448;
	fb.psm = GS_PSM_16;
	fb.address = graph_vram_allocate(fb.width, fb.height, fb.psm, GRAPH_ALIGN_PAGE);
	fb.mask = 0;

	zb.zsm = GS_PSMZ_32;
	zb.address = 0;
	zb.enable = 0;
	zb.method = ZTEST_METHOD_ALLPASS;
	zb.mask = 0;

	graph_initialize(fb.address, fb.width, fb.height, fb.psm, 0, 0);

	qword_t *init_data = (qword_t *)aligned_alloc(64, sizeof(qword_t) * 30);
	qword_t *q = init_data;

	q = draw_setup_environment(q, 0, &fb, &zb);
	q = draw_primitive_xyoffset(q, 0, 0, 0);
	q = draw_disable_tests(q, 0, &zb);

	// Clear the framebuffer
	PACK_GIFTAG(q, GIF_SET_TAG(1, 1, GIF_PRE_ENABLE, GIF_PRIM_SPRITE, GIF_FLG_PACKED, 3),
				GIF_REG_RGBAQ | (GIF_REG_XYZ2 << 4) | (GIF_REG_XYZ2 << 8));
	q++;
	q->dw[0] = (u64)((0x00) | ((u64)0x00 << 32));
	q->dw[1] = (u64)((0x00) | ((u64)0xFF << 32));
	q++;
	q->dw[0] = (u64)((((0 << 4)) | (((u64)(0 << 4)) << 32)));
	q->dw[1] = (u64)(1);
	q++;
	q->dw[0] = (u64)((((fb.width << 4)) | (((u64)(fb.height << 4)) << 32)));
	q->dw[1] = (u64)(1);
	q++;

	dma_channel_send_normal(DMA_CHANNEL_GIF, init_data, q - init_data, 0, 0);
	dma_channel_wait(DMA_CHANNEL_GIF, 0);

	free(init_data);
}

int main(void)
{
	dbgmsg("TEXA merge circuit test\n");

	init_gs();

	// Read Circuit 1 Enabled
	// Read Circuit 2 Disabled
	// CRTMD 1 (Always 1)
	// MMOD 0
	// AMOD 0
	// SLBG 1
	// ALP 0xFF
	*GS_REG_PMODE = 1 | (1 << 2) | (1 << 7) | (0xff << 8);

	// Create a plain RGBA16 texture
	RGBA16TEXEL *texels = aligned_alloc(64, sizeof(RGBA16TEXEL) * 128 * 128);
	for (size_t i = 0; i < 128 * 128; i++)
	{
		texels[i].A = 1;
		texels[i].R = 0;
		texels[i].G = 0x1f;
		texels[i].B = 0;
	}
	FlushCache(0);

	qword_t *packet = aligned_alloc(64, sizeof(qword_t) * 2048);
	qword_t *q = packet;

	dbgmsg("Finished uploading texture chain\n");

	u32 cur_ta0 = 0x80;
	u32 vsync_count = 0;
	while (1)
	{

		{ // Upload the 128x128 texture directly to our framebuffer
			q = packet;
			q = draw_texture_transfer(q, texels, 128, 128, GS_PSM_16S, 0x00, 640);
			q = draw_texture_flush(q);
			dma_channel_send_chain(DMA_CHANNEL_GIF, packet, q - packet, 0, 0);
			dma_channel_wait(DMA_CHANNEL_GIF, 0);
		}

		{ // Set the TEXA TA0 and TA1 values
			q = packet;
			PACK_GIFTAG(q, GIF_SET_TAG(1, 1, GIF_PRE_DISABLE, 0, GIF_FLG_PACKED, 1),
						GIF_REG_AD);
			q++;
			// TEXA
			q->dw[0] = GS_SET_TEXA(cur_ta0, 0, (cur_ta0 == 0x80 ? 0 : 0x80));
			q->dw[1] = GS_REG_TEXA;
			q++;
			dma_channel_send_normal(DMA_CHANNEL_GIF, packet, q - packet, 0, 0);
			dma_channel_fast_waits(DMA_CHANNEL_GIF);
		}
		graph_wait_vsync();

		if (vsync_count++ == 50)
		{
			vsync_count = 0;
			dbgmsg("Vsync 50x");
			// Swap the TA0 (and TA1) value
			cur_ta0 = (cur_ta0 == 0x80 ? 0 : 0x80);
		}
	}
	// We will never reach here, but it's nice to clean up after ourselves :^)
	free(packet);
	free(texels);

	SleepThread();
}
