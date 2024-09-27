#ifndef _GAME_BLOCKS__H
#define _GAME_BLOCKS__H

#include "game_structs.h"

#define NUM_BLOCK_TYPES 2
#define BLOCK_TYPE_EMPTY 0
#define BLOCK_TYPE_GROUND 1

#define EFFECT_FLAG_SOLID 0x00000001


void initialize_blocks(struct Sprite* sprite_array, struct Block* r_block_array);

#endif  // _GAME_BLOCKS__H