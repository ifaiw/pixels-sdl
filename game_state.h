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
};

#endif  // _GAME_STATE__H