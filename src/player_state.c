#include "player_state.h"

PlayerState player_state;

void player_state_reset(void)
{
    player_state.wx = 8;
    player_state.wy = 8;
    player_state.health = 3;
    player_state.ammo = 0;
    player_state.has_compass = 0;
    player_state.has_catapult = 0;
}
