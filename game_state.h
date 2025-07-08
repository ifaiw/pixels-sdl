#ifndef _GAME_STATE__H
#define _GAME_STATE__H

#include "game_blocks.h"
#include "game_images.h"
#include "game_sprites.h"
#include "game_structs.h"


#define WIDTH_OF_SCREEN_IN_BLOCKS 25U
#define HEIGHT_OF_SCREEN_IN_BLOCKS 18U

#define WIDTH_OF_WORLD_IN_SCREENS 4U
#define HEIGHT_OF_WORLD_IN_SCREENS 2U

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

void populate_character_for_save_from_character(struct Character* character, struct CharacterForSave* r_character_for_save);

void populate_character_from_character_for_save(struct CharacterForSave* character_for_save, struct Character* r_character);

void load_level_from_disk(struct GameState* r_game_state, const char* file_path);

void save_level_to_disk(struct GameState* game_state, const char* file_path);

#endif  // _GAME_STATE__H