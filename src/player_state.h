#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

typedef struct {
    int wx, wy;
    int health;
    int ammo;
    int has_compass;
    int has_catapult;
} PlayerState;

extern PlayerState player_state;

void player_state_reset(void);

#endif
