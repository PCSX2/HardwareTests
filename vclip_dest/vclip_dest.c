#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <sio.h>

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

typedef struct
{
	float x;
	float y;
	float z;
	float w;
} VF_REG __aligned(16);

typedef struct
{
	u8 xp1 : 1;u8 xn1 : 1;u8 yp1 : 1;u8 yn1 : 1;u8 zp1 : 1;u8 zn1 : 1;
	u8 xp2 : 1;u8 xn2 : 1;u8 yp2 : 1;u8 yn2 : 1;u8 zp2 : 1;u8 zn2 : 1;
	u8 xp3 : 1;u8 xn3 : 1;u8 yp3 : 1;u8 yn3 : 1;u8 zp3 : 1;u8 zn3 : 1;
	u8 xp4 : 1;u8 xn4 : 1;u8 yp4 : 1;u8 yn4 : 1;u8 zp4 : 1;u8 zn4 : 1;
	u16 _pad : 8;
} __attribute__((__packed__)) CLIP_REG;

void print_clip(CLIP_REG clip_reg)
{
	dbgmsg("CLIP -> - + - + - +\n"
		   "        z z y y x x\n"
		   "        %d %d %d %d %d %d\n",
		   clip_reg.zn1, clip_reg.zp1, clip_reg.yn1, clip_reg.yp1,
		   clip_reg.xn1, clip_reg.xp1);
}

int main(void)
{
	dbgmsg("VU0 VCLIP DEST BITS TEST\n");

	// Load fs and ft
	VF_REG fs = {100.0f, -201.0f, 100.00f, 0.0f};
	VF_REG ft = {200.0f, -200.0f, 0.0f, 50.0f};

	asm volatile(
		"lqc2 $vf1, %0\n"
		"vnop\n"
		"lqc2 $vf2, %1\n"
		"vnop\n" ::"m"(fs),
		"m"(ft)
		:);

	CLIP_REG clip_register_normal;
	// Try a normal VCLIP instruction
	asm volatile(
		"vclipw.xyz $vf1, $vf2\n"
		"vnop\n"
		"vnop\n"
		"vnop\n"
		"vnop\n"
		"cfc2 $t0, $18\n"
		"sw $t0, %0"
		: "=m"(clip_register_normal)::"$t0");

	dbgmsg("vclipw.xyz (Normal)");
	print_clip(clip_register_normal);

	// 0x4BC209FF -> vclipw.xyz $vf1, $vf2
	// 0x4B8209FF -> vclipw.xy $vf1, $vf2
	// 0x4B0209FF -> vclipw.x $vf1, $vf2
	CLIP_REG clip_register_modified;
	asm volatile(
		".word 0x4B8209FF\n"
		"vnop\n"
		"vnop\n"
		"vnop\n"
		"vnop\n"
		"cfc2 $t0, $18\n"
		"sw $t0, %0"
		: "=m"(clip_register_modified)::"$t0");

	dbgmsg("vclipw.xy");
	print_clip(clip_register_modified);

	SleepThread();
}
