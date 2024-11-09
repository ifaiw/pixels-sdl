#include "game_state.h"

#include <math.h>
// Just for printf
#include <stdio.h>

struct Block* get_world_block_for_location(int x, int y, struct GameState* game_state) {
    // TODO is x or y outside the world bounds handled properly? Just force the world to have a rectangle of blocks all around the outmost edge?
    if (x < 0 || y < 0) {
        return NULL;
    }

    int block_x = x / BLOCK_WIDTH_IN_PIXELS;
    int block_y = y / BLOCK_HEIGHT_IN_PIXELS;
    printf("x=%d y=%d block_x=%d block_y=%d\n", x, y, block_x, block_y);
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
    int bottom_left_x_floor = floor(game_state->character.x_bottom_left);
    int bottom_left_y_floor = floor(game_state->character.y_inverted_bottom_left);
    int right_pixel = bottom_left_x_floor + game_state->character.width;
    int just_below_pixel = bottom_left_y_floor - 1;

    printf("y_inverted_bottom_left=%f bottom_left_y_floor=%d just_below_pixel=%d\n", game_state->character.y_inverted_bottom_left, bottom_left_y_floor, just_below_pixel);

    struct Block* bottom_left = get_world_block_for_location(bottom_left_x_floor, just_below_pixel, game_state);
    // TODO just for testing make the blocks below look different
    // bottom_left->sprite = game_state->base_sprites[SPRITE_TYPE_GROUND_TEST];
    printf("effects flag for bottom_left is %d\n", bottom_left->effects_flags);
    if (bottom_left->effects_flags & EFFECT_FLAG_SOLID) {
        return true;
    }
    struct Block* bottom_right = get_world_block_for_location(right_pixel, just_below_pixel, game_state);
    // TODO just for testing make the blocks below look different
    // bottom_right->sprite = game_state->base_sprites[SPRITE_TYPE_GROUND_TEST2];
    printf("effects flag for bottom_right is %d\n", bottom_left->effects_flags);
    return bottom_right->effects_flags & EFFECT_FLAG_SOLID;
}