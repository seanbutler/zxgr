#ifndef WORLD_API_H
#define WORLD_API_H

typedef struct {
    const char          *tiles;    /* tile palette, row-major within each tile */
    const unsigned char *map;      /* tx,ty,ti triples + 0xFF sentinel         */
    unsigned char        tile_w;
    unsigned char        tile_h;
} World;

void world_draw(const World *w, int scroll_x, int scroll_y);
char world_char_at(const World *w, int wx, int wy);

#endif /* WORLD_API_H */
