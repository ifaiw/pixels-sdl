#ifndef _GAME_BLOCKS__H
#define _GAME_BLOCKS__H

#include "game_structs.h"

#define NUM_BLOCK_TYPES 4
#define BLOCK_TYPE_EMPTY 0
#define BLOCK_TYPE_GROUND 1
#define BLOCK_TYPE_LADDER 2
#define BLOCK_TYPE_TOILET 3

#define EFFECT_FLAG_SOLID 0x00000001
#define EFFECT_FLAG_CLIMABLE 0x00000010

#define BLOCK_WIDTH_IN_PIXELS 48
#define BLOCK_HEIGHT_IN_PIXELS 48

void initialize_blocks(struct Sprite* sprite_array, struct Block* r_block_array);

// TODO used?
struct Block* get_block_for_block_xy(struct XY block_xy, struct Block* r_block_a);

// TODO used?
struct XY get_block_xy_for_pixel_xy(int pixel_x, int pixel_y);

#endif  // _GAME_BLOCKS__H