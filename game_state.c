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