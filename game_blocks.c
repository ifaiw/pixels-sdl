#include "game_blocks.h"

#include "game_sprites.h"

// IMPLEMENTS
void initialize_blocks(struct Sprite* sprite_array, struct Block* r_block_array) {
    r_block_array[BLOCK_TYPE_EMPTY].type = BLOCK_TYPE_EMPTY;
    r_block_array[BLOCK_TYPE_EMPTY].effects_flags = 0;
    r_block_array[BLOCK_TYPE_EMPTY].sprite_index = SPRITE_TYPE_EMPTY;

    r_block_array[BLOCK_TYPE_GROUND].type = BLOCK_TYPE_GROUND;
    r_block_array[BLOCK_TYPE_GROUND].effects_flags = EFFECT_FLAG_SOLID;
    r_block_array[BLOCK_TYPE_GROUND].sprite_index = SPRITE_TYPE_GROUND_BLOCKED_ALL_SIDES;

    r_block_array[BLOCK_TYPE_LADDER].type = BLOCK_TYPE_LADDER;
    r_block_array[BLOCK_TYPE_LADDER].effects_flags = EFFECT_FLAG_CLIMABLE;
    r_block_array[BLOCK_TYPE_LADDER].sprite_index = SPRITE_TYPE_LADDER;

    r_block_array[BLOCK_TYPE_TOILET].type = BLOCK_TYPE_TOILET;
    r_block_array[BLOCK_TYPE_TOILET].effects_flags = 0;
    r_block_array[BLOCK_TYPE_TOILET].sprite_index = SPRITE_TYPE_TOILET;
}
