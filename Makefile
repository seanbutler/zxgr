Z88DK      = $(HOME)/Apps/z88dk
ZESARUX    = $(HOME)/Apps/ZEsarUX-13.0/zesarux

export PATH    := $(Z88DK)/bin:$(PATH)
export ZCCCFG  := $(Z88DK)/lib/config/

TARGET  = gr
SRC     = gr.c sprite.c world.c

.PHONY: all run clean

all: $(TARGET).P

$(TARGET).P: $(SRC)
	zcc +zx81 -create-app -o $(TARGET) $(SRC)

run: $(TARGET).P
	$(ZESARUX) --machine ZX81 $(TARGET).P &

clean:
	rm -f $(TARGET).P $(TARGET).bin $(TARGET)_*.bin *.lis *.sym *.map zcc_opt.def
