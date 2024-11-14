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

    struct WorldRules world_rules;

    long current_frame;
    long current_time_in_micros;

    // Pixel offset of the top-left corner of the area where blocks are rendered in the window
    int blocks_area_offset_x;
    int blocks_area_offset_y;
};

struct Block* get_world_block_for_location(int x, int y, struct GameState* game_state);

struct XY get_bottom_left_world_pixel_for_block(struct Block* block);

bool is_on_ground(struct GameState* game_state);

void load_world_rules_from_file(struct WorldRules* r_world_rules);

#endif  // _GAME_STATE__H