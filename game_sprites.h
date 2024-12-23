#ifndef _GAME_SPRITES__H
#define _GAME_SPRITES__H

#include <inttypes.h>

#include "game_structs.h"

// Height and width in pixels
// TODO Need to rename this at least
#define SPRITE_WIDTH 48
#define SPRITE_HEIGHT 48

#define NUM_SPRITE_TYPES 26
#define SPRITE_TYPE_EMPTY 0
#define SPRITE_TYPE_GROUND 1
#define SPRITE_TYPE_ORC_STAND_RIGHT 2
#define SPRITE_TYPE_ORC_WALK_RIGHT_1 3
#define SPRITE_TYPE_ORC_WALK_RIGHT_2 4
#define SPRITE_TYPE_ORC_WALK_RIGHT_3 5
#define SPRITE_TYPE_ORC_WALK_RIGHT_4 6
#define SPRITE_TYPE_ORC_WALK_RIGHT_5 7
#define SPRITE_TYPE_ORC_WALK_RIGHT_6 8
#define SPRITE_TYPE_ORC_WALK_RIGHT_7 9

#define SPRITE_TYPE_MUSHROOM_STAND_RIGHT 10
#define SPRITE_TYPE_MUSHROOM_WALK_RIGHT_1 11
#define SPRITE_TYPE_MUSHROOM_WALK_RIGHT_2 12
#define SPRITE_TYPE_MUSHROOM_WALK_RIGHT_3 13
#define SPRITE_TYPE_MUSHROOM_WALK_RIGHT_4 14
#define SPRITE_TYPE_MUSHROOM_WALK_RIGHT_5 15
#define SPRITE_TYPE_MUSHROOM_WALK_RIGHT_6 16
#define SPRITE_TYPE_MUSHROOM_WALK_RIGHT_7 17

#define SPRITE_TYPE_CAT_STAND_RIGHT 18
#define SPRITE_TYPE_CAT_WALK_RIGHT_1 19
#define SPRITE_TYPE_CAT_WALK_RIGHT_2 20
#define SPRITE_TYPE_CAT_WALK_RIGHT_3 21
#define SPRITE_TYPE_CAT_WALK_RIGHT_4 22
#define SPRITE_TYPE_CAT_WALK_RIGHT_5 23


#define SPRITE_TYPE_GROUND_TEST 24
#define SPRITE_TYPE_GROUND_TEST2 25


void initialize_sprites(struct ImageInfo* image_array, struct Sprite* r_sprite_array);

#endif  // _GAME_SPRITES__H