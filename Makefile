Z88DK      = $(HOME)/Apps/z88dk
ZESARUX    = $(HOME)/Apps/ZEsarUX-13.0/zesarux

export PATH    := $(Z88DK)/bin:$(PATH)
export ZCCCFG  := $(Z88DK)/lib/config/

TARGET    = gr
SRC       = src/main.c src/lib/core/dfile.c src/lib/gfx/sprite.c src/world.c src/lib/math/rng.c src/shape_player.c src/shape_enemy.c src/shape_boon.c src/boon_map.c src/boon.c
INCLUDES  = -Isrc -Isrc/lib/core -Isrc/lib/gfx -Isrc/lib/math -Isrc/lib/input
BUILD     = build
WORLD_H   = src/world_bg.h
WORLD_PNG = assets/world.png

.PHONY: all run clean

all: $(BUILD)/$(TARGET).P

$(BUILD)/$(TARGET).P: $(SRC) $(WORLD_H) | $(BUILD)
	zcc +zx81 -O2 --opt-code-speed $(INCLUDES) -create-app -o $(BUILD)/$(TARGET) $(SRC)

$(WORLD_H): $(WORLD_PNG)
	python3 tools/png2zx81.py $(WORLD_PNG) $(WORLD_H) world_bg

$(BUILD):
	mkdir -p $(BUILD)

run: $(BUILD)/$(TARGET).P
	$(ZESARUX) --machine ZX81 $(BUILD)/$(TARGET).P &

clean:
	rm -f $(BUILD)/$(TARGET).P $(BUILD)/$(TARGET).bin $(BUILD)/$(TARGET)_*.bin *.lis *.sym *.map zcc_opt.def
