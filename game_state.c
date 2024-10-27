#include "game_state.h"

// For NULL
#include <string.h>

struct Block* get_world_block_for_location(int x, int y, struct GameState* game_state) {
    // TODO is x or y outside the world bounds handled properly? Just force the world to have a rectangle of blocks all around the outmost edge?
    if (x < 0 || y < 0) {
        return NULL;
    }

    int block_x = x / BLOCK_WIDTH_IN_PIXELS;
    int block_y = y / BLOCK_HEIGHT_IN_PIXELS;
    if (block_x >= WORLD_BLOCKS_WIDTH || block_y >= WORLD_BLOCKS_HEIGHT) {
        return NULL;
    }

    return &game_state->world_blocks[block_y * WORLD_BLOCKS_WIDTH + block_x];
}

// Returns inverted y I think?
struct XY get_bottom_left_world_pixel_for_block(struct Block* block) {
    struct XY xy;
    xy.x = block->world_x * BLOCK_WIDTH_IN_PIXELS;
    xy.y = block->world_y * BLOCK_HEIGHT_IN_PIXELS;
    return xy;
}

bool is_on_ground(struct GameState* game_state) {
    // TODO is rounding a problem here? Do we need to specifically check if we should floor or ceil?
    // Same question applies to collision detection I assume
    int bottom_left_x_rounded = round(game_state->character.x_bottom_left);
    int bottom_left_y_rounded = round(game_state->character.y_inverted_bottom_left);
    int right_pixel = bottom_left_x_rounded + game_state->character.width;
    int just_below_pixel = bottom_left_y_rounded - 1;

    struct Block* bottom_left = get_world_block_for_location(bottom_left_x_rounded, just_below_pixel, game_state);
    if (bottom_left->effects_flags & EFFECT_FLAG_SOLID) {
        return true;
    }
    struct Block* bottom_right = get_world_block_for_location(right_pixel, just_below_pixel, game_state);
    return bottom_right->effects_flags & EFFECT_FLAG_SOLID;
}