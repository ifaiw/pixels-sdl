#ifndef _GAME_STATE__H
#define _GAME_STATE__H

#include "game_blocks.h"
#include "game_images.h"
#include "game_sprites.h"
#include "game_structs.h"


#define WORLD_BLOCKS_WIDTH  25
#define WORLD_BLOCKS_HEIGHT 18

struct GameState {
    struct ImageInfo base_bmp_images[NUM_BMP_IMAGES];
    struct Sprite base_sprites[NUM_SPRITE_TYPES];
    struct Block base_blocks[NUM_BLOCK_TYPES];

    struct Block world_blocks[WORLD_BLOCKS_WIDTH * WORLD_BLOCKS_HEIGHT];

    struct Character character;

    long current_frame;
    long current_time_in_micros;
};

struct Block* get_world_block_for_location(int x, int y, struct GameState* game_state);

struct XY get_bottom_left_world_pixel_for_block(struct Block* block);

#endif  // _GAME_STATE__H