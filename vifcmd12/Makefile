EE_OBJS	= vifcmd12.o micro/loop.o
EE_BIN = vifcmd12.elf
EE_LIBS = -lkernel
EE_DVP = dvp-as
EE_VCL = vcl

all: $(EE_BIN)

%.o: %.vsm
	$(EE_DVP) $< -o $@

clean:
	rm -f $(EE_BIN) $(EE_OBJS) 

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

wsl: $(EE_BIN)
	$(PCSX2) --elf="$(shell wslpath -w $(shell pwd))/$(EE_BIN)"

emu: $(EE_BIN)
	$(PCSX2) --elf="$(shell pwd)/$(EE_BIN)"

reset:
	ps2client reset
	ps2client netdump

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal