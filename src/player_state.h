#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

typedef struct {
    int health;
    int ammo;
    int has_compass;
} PlayerState;

extern PlayerState player_state;

#endif
