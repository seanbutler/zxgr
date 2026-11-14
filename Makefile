Z88DK      = $(HOME)/Apps/z88dk
ZESARUX    = $(HOME)/Apps/ZEsarUX-13.0/zesarux

export PATH    := $(Z88DK)/bin:$(PATH)
export ZCCCFG  := $(Z88DK)/lib/config/

TARGET  = gr
SRC     = src/gr.c src/sprite.c src/world.c src/rng.c
BUILD   = build

.PHONY: all run clean

all: $(BUILD)/$(TARGET).P

$(BUILD)/$(TARGET).P: $(SRC) | $(BUILD)
	zcc +zx81 -O2 -create-app -o $(BUILD)/$(TARGET) $(SRC)

$(BUILD):
	mkdir -p $(BUILD)

run: $(BUILD)/$(TARGET).P
	$(ZESARUX) --machine ZX81 $(BUILD)/$(TARGET).P &

clean:
	rm -f $(BUILD)/$(TARGET).P $(BUILD)/$(TARGET).bin $(BUILD)/$(TARGET)_*.bin *.lis *.sym *.map zcc_opt.def
