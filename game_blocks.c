#include "game_blocks.h"

#include "game_sprites.h"

// IMPLEMENTS
void initialize_blocks(struct Sprite* sprite_array, struct Block* r_block_array) {
    r_block_array[BLOCK_TYPE_EMPTY].type = BLOCK_TYPE_EMPTY;
    r_block_array[BLOCK_TYPE_EMPTY].effects_flags = 0;
    r_block_array[BLOCK_TYPE_EMPTY].sprite = sprite_array[SPRITE_TYPE_EMPTY];

    r_block_array[BLOCK_TYPE_GROUND].type = BLOCK_TYPE_GROUND;
    r_block_array[BLOCK_TYPE_GROUND].effects_flags = EFFECT_FLAG_SOLID;
    r_block_array[BLOCK_TYPE_GROUND].sprite = sprite_array[SPRITE_TYPE_GROUND];
}