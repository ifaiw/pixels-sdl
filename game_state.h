#ifndef _GAME_STATE__H
#define _GAME_STATE__H

#include "game_blocks.h"
#include "game_images.h"
#include "game_sprites.h"
#include "game_structs.h"


#define WIDTH_OF_SCREEN_IN_BLOCKS 25
#define HEIGHT_OF_SCREEN_IN_BLOCKS 18

#define WIDTH_OF_WORLD_IN_SCREENS 4
#define HEIGHT_OF_WORLD_IN_SCREENS 2

#define WIDTH_OF_WORLD_IN_BLOCKS WIDTH_OF_SCREEN_IN_BLOCKS * WIDTH_OF_WORLD_IN_SCREENS
#define HEIGHT_OF_WORLD_IN_BLOCKS HEIGHT_OF_SCREEN_IN_BLOCKS * HEIGHT_OF_WORLD_IN_SCREENS

struct GameState {
    struct ImageInfo base_bmp_images[NUM_BMP_IMAGES];
    struct Sprite base_sprites[NUM_SPRITE_TYPES];
    struct Block base_blocks[NUM_BLOCK_TYPES];

    // Layout of world_blocks
    // Easiest layout I think is:
    // Bottom/lowest row of blocks is at y = 0
    // world_blocks starts with entire first y=0 row of the world, then y=1 1 row, and so on
    struct Block world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * HEIGHT_OF_WORLD_IN_BLOCKS];

    struct Character character;

    struct WorldRules world_rules;

    struct CharacterSprite character_sprite;

    long current_frame;
    long current_time_in_micros;
};

struct Block* get_world_block_for_location(int x, int y, struct GameState* game_state);

struct XY get_bottom_left_world_pixel_for_block(struct Block* block);

bool is_on_ground(struct GameState* game_state);

bool is_on_climable(struct GameState* game_state);

void load_world_rules_from_file(struct WorldRules* r_world_rules);

void update_ground_images(struct GameState* game_state);

#endif  // _GAME_STATE__H