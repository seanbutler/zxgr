# TODO

## Gameplay
- [x] Player movement — read keyboard input and move `px`/`py`  DONE
- [x] Scroll the world as the player moves toward the edge  DONE (page-flip)
- [ ] Collision detection between player and enemies
- [x] Enemy vertical movement (currently only dx, no dy)  DONE
- [ ] Boon collection — detect player overlap with boon, apply effect, mark BOON_NONE
- [ ] Health system — player health value, damage from bat contact, game over condition
- [ ] Ammo system — ammo value, firing mechanic (KEY_ACTION)
- [ ] Compass boon — reveal map or show direction to exit/goal
- [ ] Player death — show game_over screen, restart or return to title

## Enemies
- [ ] Populate enemy_map.c with real spawn positions for all world screens
- [ ] Bat damage — contact with player reduces health

## World
- [ ] Populate boon_map.c with real positions for all collectables

## Graphics
- [x] Player sprite art — currently a placeholder shape  DONE
- [x] Enemy sprite variations (more than one enemy design)  DONE (bat frames)
- [x] Animate sprites across frames (cycle between frames)  DONE (bat flap)

## HUD
- [ ] Wire health/ammo display to real values (currently hardcoded to 10/6)
- [ ] HUD icons — draw shape_boon_health/ammo/compass icons next to values

## Polish
- [ ] Title screen — wait for key then transition cleanly (dfile_clear before gameplay)
- [ ] Game over screen — triggered on player death
- [ ] Sound — ZX81 has no sound hardware (speaker only via border), low priority

## Engine
- [x] Clip sprites at screen edges (currently clamp but no partial-draw)  DONE
- [ ] Dirty-rect optimisation — only redraw changed cells
- [ ] Investigate flicker: double-buffer or draw-order tuning
- [x] Player environment collision  DONE
- [x] Enemy environment collision  DONE

## Technical
- [ ] Verify all enemy_map and boon_map coordinates land in open cave areas
- [ ] In png2zx81 asset conversion, fail generation if boon/enemy marker colors are placed inside wall tiles
- [ ] RLE compress title/game_over screens if ROM gets tight (~420 B saving)
- [ ] Experiment with world tile size parameters (try 3×3, 6×6, 8×8) to find compression sweet spot — measure unique tile count and total .h size; artist can also tweak the PNG to reduce unique tiles where the map is inefficient

## Tooling
- [x] Document the asset pipeline in `tools/` (how PNG → .h tile data)  DONE
- [x] Add a `make assets` target that regenerates the .h files  DONE (make auto-rebuilds)
- [x] Title screen and game over screen PNG → ZX81 converter (png2screen.py)  DONE

## Housekeeping
- [ ] Remove `autosnap-*.zsf` from the repo (add to .gitignore)
- [ ] Add `.gitignore` for build artifacts (`*.P`, `*.bin`, `*.lis`, etc.)

## Reviews
- [ ] Efficiency / optimisation review — Z80 hot paths, unnecessary work per frame, ROM/RAM usage
- [ ] Bad code smells & architecture review — coupling, naming, responsibilities, anything that will bite later
- [ ] Profile tile map rendering and enemy update loop — verify sparse map scan and per-frame enemy work is not burning cycles; revisit if frame rate suffers
- [ ] Player vs enemy collision detection — currently absent; when added, ensure it is cheap (AABB on small sprite counts, not per-char scan)
- [ ] Player vs boon collision detection — same concern; boon_map scan every frame could be O(n) over all boons in the world, should be bounded to current screen only
