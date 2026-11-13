# TODO

## Gameplay
- [ ] Player movement — read keyboard input and move `px`/`py`
- [ ] Scroll the world as the player moves toward the edge
- [ ] Collision detection between player and enemies
- [ ] Enemy vertical movement (currently only dx, no dy)

## Graphics
- [ ] Player sprite art — currently a placeholder shape
- [ ] Enemy sprite variations (more than one enemy design)
- [ ] Animate sprites across frames (cycle between frames)

## Engine
- [ ] Clip sprites at screen edges (currently clamp but no partial-draw)
- [ ] Dirty-rect optimisation — only redraw changed cells
- [ ] Investigate flicker: double-buffer or draw-order tuning

## Tooling
- [ ] Document the asset pipeline in `tools/` (how PNG → .h tile data)
- [ ] Add a `make assets` target that regenerates the .h files

## Housekeeping
- [ ] Remove `autosnap-*.zsf` from the repo (add to .gitignore)
- [ ] Add `.gitignore` for build artifacts (`*.P`, `*.bin`, `*.lis`, etc.)
