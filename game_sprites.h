#ifndef _GAME_SPRITES__H
#define _GAME_SPRITES__H

#include <inttypes.h>

#include "game_structs.h"

// Height and width in pixels
#define SPRITE_WIDTH 48
#define SPRITE_HEIGHT 48

#define NUM_SPRITE_TYPES 3
#define SPRITE_TYPE_EMPTY 0
#define SPRITE_TYPE_GROUND 1
#define SPRITE_TYPE_ORC_STAND 2


void initialize_sprites(struct ImageInfo* image_array, struct Sprite* r_sprite_array);

#endif  // _GAME_SPRITES__H