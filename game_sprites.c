#include "game_sprites.h"

#include "game_blocks.h"
#include "game_images.h"
#include "graphics_constants.h"

// TODO maybe make version of this method that just takes one y value, uses it as near the y-middle of the sprites,
// then searches up and down to find top and bottom horizontal lines that are blank to use as start y and max y
// IMPLEMENTS
int initialize_sprites_automatic(struct ImageInfo* image, int pixel_start_x, int pixel_start_y, int max_pixel_x, int max_pixel_y, uint16_t num_sprites_to_initialize, uint16_t first_sprite_index, struct Sprite* r_sprite_array) {
    int pixel_starts[num_sprites_to_initialize];
    int x_start[num_sprites_to_initialize];
    int y_start[num_sprites_to_initialize];
    int x_end[num_sprites_to_initialize];
    int y_end[num_sprites_to_initialize];

    for (int y_index = pixel_start_y; y_index <= max_pixel_y; ++y_index) {
        if ((image->pixels[pixel_start_x + y_index * image->width] & ALPHA) > 0) {
            printf("First column of pixels in initialize_sprites_automatic is not empty!\n");
            return -1;
        }
    }

    int sprite_index = 0;
    int x_index = pixel_start_x + 1;
    while (x_index <= max_pixel_x && sprite_index < num_sprites_to_initialize) {

        // Find the left-most/first column of the next sprite
        while (x_index <= max_pixel_x) {
            bool found_pixel = false;
            for (int y_index = pixel_start_y; y_index <= max_pixel_y; ++y_index) {
                if ((image->pixels[x_index + y_index * image->width] & ALPHA) > 0) {
                    y_start[sprite_index] = y_index;
                    found_pixel = true;
                    break;
                }
            }

            x_index += 1;
            if (found_pixel) {
                x_start[sprite_index] = x_index - 1;
                break;
            }
        }

        // Find the right-most/last column of the current sprite
        while (x_index <= max_pixel_x) {
            bool found_pixel = false;
            for (int y_index = pixel_start_y; y_index <= max_pixel_y; ++y_index) {
                if ((image->pixels[x_index + y_index * image->width] & ALPHA) > 0) {
                    found_pixel = true;
                    if (y_index < y_start[sprite_index]) {
                        y_start[sprite_index] = y_index;
                    }
                    break;
                }
            }

            x_index += 1;
            if (!found_pixel) {
                x_end[sprite_index] = x_index - 1;
                break;
            }
        }

        sprite_index += 1;
    }

    if (x_index > max_pixel_x) {
        printf("Did not find all expected sprites in initialize_sprites_automatic!\n");
            return -2;
    }

    // Found the top y for all sprites above but still need to bottom y
    sprite_index = 0;
    for (int x_index = x_start[0]; x_index <= x_end[num_sprites_to_initialize - 1]; ++x_index) {
        y_end[sprite_index] = y_start[sprite_index];
        if (x_index > x_end[sprite_index]) {
            sprite_index += 1;
            x_index = x_start[sprite_index];
            y_end[sprite_index] = y_start[sprite_index];
        }

        for (int y_index = max_pixel_y; y_index > y_end[sprite_index]; --y_index) {
            if (((image->pixels[x_index + (y_index - 1) * image->width] & ALPHA) > 0) && (y_index - 1) > y_end[sprite_index]) {
                y_end[sprite_index] = y_index - 1;
            }
        }
    }

    for (sprite_index = 0; sprite_index < num_sprites_to_initialize; ++sprite_index) {
        (r_sprite_array + sprite_index)->sprite_index = first_sprite_index + sprite_index;
        (r_sprite_array + sprite_index)->image_source_pitch_in_pixels = image->width;
        (r_sprite_array + sprite_index)->pixels_start = x_start[sprite_index] + y_start[sprite_index] * image->width;
        (r_sprite_array + sprite_index)->height = y_end[sprite_index] - y_start[sprite_index] + 1;
        (r_sprite_array + sprite_index)->width = x_end[sprite_index] - x_start[sprite_index] + 1;
    }

    return 0;
}

// IMPLEMENTS
void initialize_sprites(struct ImageInfo* image_array, struct Sprite* r_sprite_array) {
    r_sprite_array[SPRITE_TYPE_EMPTY].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_EMPTY].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_EMPTY].pixels_start = image_array[IMAGE_INDEX_BLANK].pixels;
    r_sprite_array[SPRITE_TYPE_EMPTY].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_BLANK].width;

    r_sprite_array[SPRITE_TYPE_GROUND_BLOCKED_ALL_SIDES].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_BLOCKED_ALL_SIDES].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_BLOCKED_ALL_SIDES].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 48 * image_array[IMAGE_INDEX_SOLIDS_2].width + 48;
    r_sprite_array[SPRITE_TYPE_GROUND_BLOCKED_ALL_SIDES].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 48 + 48 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 48 + 48 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 48 + 48 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_UP].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_UP].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_UP].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 192 + 0 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_UP].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_UP].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_UP].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_UP].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 96 + 0 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_UP].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_UP].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_UP].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_UP].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 144 + 0 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_UP].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_UP].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_UP].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_UP].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 48 + 0 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_UP].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_DOWN].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_DOWN].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_DOWN].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 48 + 48 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_DOWN].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_DOWN].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_DOWN].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_DOWN].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 320 + 0 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_DOWN].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_DOWN].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_DOWN].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_DOWN].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 368 + 0 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_DOWN].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_DOWN].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_DOWN].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_DOWN].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 320 + 48 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_DOWN].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_UP_DOWN].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_UP_DOWN].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_UP_DOWN].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 192 + 0 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_UP_DOWN].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_UP_DOWN].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_UP_DOWN].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_UP_DOWN].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 96 + 0 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_UP_DOWN].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_UP_DOWN].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_UP_DOWN].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_UP_DOWN].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 144 + 0 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_RIGHT_UP_DOWN].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_UP_DOWN].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_UP_DOWN].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_UP_DOWN].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 48 + 0 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_UP_DOWN].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;

    r_sprite_array[SPRITE_TYPE_LADDER].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_LADDER].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_LADDER].pixels_start = image_array[IMAGE_INDEX_SOLIDS_2].pixels + 416 + 0 * image_array[IMAGE_INDEX_SOLIDS_2].width;
    r_sprite_array[SPRITE_TYPE_LADDER].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_2].width;



    // r_sprite_array[SPRITE_TYPE_ORC_STAND_LEFT].height = 28;
    // r_sprite_array[SPRITE_TYPE_ORC_STAND_LEFT].width = 42;
    // r_sprite_array[SPRITE_TYPE_ORC_STAND_LEFT].pixels_start = image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels + (166 * image_array[IMAGE_INDEX_ORC_1_RIGHT].width) + 120;
    // r_sprite_array[SPRITE_TYPE_ORC_STAND_LEFT].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_ORC_1_RIGHT].width;

    r_sprite_array[SPRITE_TYPE_MUSHROOM_STAND_RIGHT].height = 32;
    r_sprite_array[SPRITE_TYPE_MUSHROOM_STAND_RIGHT].width = 26;
    r_sprite_array[SPRITE_TYPE_MUSHROOM_STAND_RIGHT].pixels_start = image_array[IMAGE_INDEX_MUSHROOM_RIGHT].pixels + 1;
    r_sprite_array[SPRITE_TYPE_MUSHROOM_STAND_RIGHT].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_MUSHROOM_RIGHT].width;

    int mushroom_width = 28;
    int mushroom_height = 32;
    // The gaps are not uniform, so hardcode starting indices  int mushroom_gap = 4;
    int mushroom_starting_x_values[7] = {34, 68, 98, 130, 162, 196, 226};
    for (int walk_index = 0; walk_index < 7; ++walk_index) {
        r_sprite_array[SPRITE_TYPE_MUSHROOM_WALK_RIGHT_1 + walk_index].height = mushroom_height;
        r_sprite_array[SPRITE_TYPE_MUSHROOM_WALK_RIGHT_1 + walk_index].width = mushroom_width;
        r_sprite_array[SPRITE_TYPE_MUSHROOM_WALK_RIGHT_1 + walk_index].pixels_start = image_array[IMAGE_INDEX_MUSHROOM_RIGHT].pixels + mushroom_starting_x_values[walk_index];
        r_sprite_array[SPRITE_TYPE_MUSHROOM_WALK_RIGHT_1 + walk_index].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_MUSHROOM_RIGHT].width;
    }
    // TODO Do we need to do something to better handle the climbing height being higher than the walking height?
    // Should the height vary across different climbing frames?
    int mushroom_climbing_height = 36;
    for (int climb_index = 0; climb_index < 12; ++climb_index) {
        r_sprite_array[SPRITE_TYPE_MUSHROOM_CLIMB_1 + climb_index].height = mushroom_climbing_height;
        r_sprite_array[SPRITE_TYPE_MUSHROOM_CLIMB_1 + climb_index].width = mushroom_width;
        r_sprite_array[SPRITE_TYPE_MUSHROOM_CLIMB_1 + climb_index].pixels_start = image_array[IMAGE_INDEX_MUSHROOM_RIGHT].pixels + 98 + (climb_index * (mushroom_width + 2)) + (94 * image_array[IMAGE_INDEX_MUSHROOM_RIGHT].width);
        r_sprite_array[SPRITE_TYPE_MUSHROOM_CLIMB_1 + climb_index].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_MUSHROOM_RIGHT].width;
    }

    initialize_sprites_automatic(image_array + IMAGE_INDEX_WORM, 53, 3, )

    r_sprite_array[SPRITE_TYPE_WORM_PULL_FORWARD_1].height = 10;
    r_sprite_array[SPRITE_TYPE_WORM_PULL_FORWARD_1].width = 22;
    r_sprite_array[SPRITE_TYPE_WORM_PULL_FORWARD_1].pixels_start = image_array[IMAGE_INDEX_WORM].pixels + 53 + (3 * image_array[IMAGE_INDEX_WORM].width);
    r_sprite_array[SPRITE_TYPE_WORM_PULL_FORWARD_1].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_WORM].width;

    r_sprite_array[SPRITE_TYPE_WORM_PULL_FORWARD_1].height = 10;
    r_sprite_array[SPRITE_TYPE_WORM_PULL_FORWARD_1].width = 22;
    r_sprite_array[SPRITE_TYPE_WORM_PULL_FORWARD_1].pixels_start = image_array[IMAGE_INDEX_WORM].pixels + 53 + (3 * image_array[IMAGE_INDEX_WORM].width);
    r_sprite_array[SPRITE_TYPE_WORM_PULL_FORWARD_1].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_WORM].width;

    int cat_width = 26;
    int cat_gap = 6;
    r_sprite_array[SPRITE_TYPE_CAT_STAND_RIGHT].height = 32;
    r_sprite_array[SPRITE_TYPE_CAT_STAND_RIGHT].width = cat_width;
    r_sprite_array[SPRITE_TYPE_CAT_STAND_RIGHT].pixels_start = image_array[IMAGE_INDEX_CAT_RIGHT].pixels + 2;
    r_sprite_array[SPRITE_TYPE_CAT_STAND_RIGHT].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_CAT_RIGHT].width;
    for (int walk_index = 0; walk_index < 5; ++walk_index) {
        r_sprite_array[SPRITE_TYPE_CAT_WALK_RIGHT_1 + walk_index].height = 32;
        r_sprite_array[SPRITE_TYPE_CAT_WALK_RIGHT_1 + walk_index].width = cat_width;
        r_sprite_array[SPRITE_TYPE_CAT_WALK_RIGHT_1 + walk_index].pixels_start = image_array[IMAGE_INDEX_CAT_RIGHT].pixels + 2 + (cat_width + cat_gap) * (walk_index + 1);
        r_sprite_array[SPRITE_TYPE_CAT_WALK_RIGHT_1 + walk_index].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_CAT_RIGHT].width;
    }

    for (int sprite_index = 0; sprite_index < NUM_SPRITE_TYPES; ++sprite_index) {
        r_sprite_array[sprite_index].sprite_index = sprite_index;
    }

    r_sprite_array[SPRITE_TYPE_TOILET].height = BLOCK_HEIGHT_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_TOILET].width = BLOCK_WIDTH_IN_PIXELS;
    r_sprite_array[SPRITE_TYPE_TOILET].pixels_start = image_array[IMAGE_INDEX_TOILET].pixels + 0 + 0 * image_array[IMAGE_INDEX_TOILET].width;
    r_sprite_array[SPRITE_TYPE_TOILET].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_TOILET].width;

}