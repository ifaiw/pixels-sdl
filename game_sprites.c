#include "game_sprites.h"

#include "game_images.h"

// IMPLEMENTS
void initialize_sprites(struct ImageInfo* image_array, struct Sprite* r_sprite_array) {
    r_sprite_array[SPRITE_TYPE_EMPTY].height = SPRITE_HEIGHT;
    r_sprite_array[SPRITE_TYPE_EMPTY].width = SPRITE_WIDTH;
    r_sprite_array[SPRITE_TYPE_EMPTY].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_EMPTY].pixels_start = image_array[IMAGE_INDEX_BLANK].pixels;
    r_sprite_array[SPRITE_TYPE_EMPTY].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_BLANK].width;

    r_sprite_array[SPRITE_TYPE_GROUND].height = SPRITE_HEIGHT;
    r_sprite_array[SPRITE_TYPE_GROUND].width = SPRITE_WIDTH;
    r_sprite_array[SPRITE_TYPE_GROUND].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_GROUND].pixels_start = image_array[IMAGE_INDEX_SOLIDS_1].pixels + 48 * image_array[IMAGE_INDEX_SOLIDS_1].width;
    r_sprite_array[SPRITE_TYPE_GROUND].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_1].width;

    // r_sprite_array[SPRITE_TYPE_ORC_STAND_LEFT].height = 28;
    // r_sprite_array[SPRITE_TYPE_ORC_STAND_LEFT].width = 42;
    // r_sprite_array[SPRITE_TYPE_ORC_STAND_LEFT].flip_left_to_right = true;
    // r_sprite_array[SPRITE_TYPE_ORC_STAND_LEFT].pixels_start = image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels + (166 * image_array[IMAGE_INDEX_ORC_1_RIGHT].width) + 120;
    // r_sprite_array[SPRITE_TYPE_ORC_STAND_LEFT].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_ORC_1_RIGHT].width;

    r_sprite_array[SPRITE_TYPE_MUSHROOM_STAND_RIGHT].height = 32;
    r_sprite_array[SPRITE_TYPE_MUSHROOM_STAND_RIGHT].width = 26;
    r_sprite_array[SPRITE_TYPE_MUSHROOM_STAND_RIGHT].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_MUSHROOM_STAND_RIGHT].pixels_start = image_array[IMAGE_INDEX_MUSHROOM_RIGHT].pixels + 1;
    r_sprite_array[SPRITE_TYPE_MUSHROOM_STAND_RIGHT].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_MUSHROOM_RIGHT].width;

    int mushroom_width = 26;
    // The gaps are not uniform, so hardcode starting indices  int mushroom_gap = 4;
    int mushroom_starting_x_values[7] = {34, 68, 98, 130, 162, 196, 226};
    for (int walk_index = 0; walk_index < 7; ++walk_index) {
        r_sprite_array[SPRITE_TYPE_MUSHROOM_WALK_RIGHT_1 + walk_index].height = 32;
        r_sprite_array[SPRITE_TYPE_MUSHROOM_WALK_RIGHT_1 + walk_index].width = mushroom_width;
        r_sprite_array[SPRITE_TYPE_MUSHROOM_WALK_RIGHT_1 + walk_index].flip_left_to_right = false;
        r_sprite_array[SPRITE_TYPE_MUSHROOM_WALK_RIGHT_1 + walk_index].pixels_start = image_array[IMAGE_INDEX_MUSHROOM_RIGHT].pixels + mushroom_starting_x_values[walk_index];
        r_sprite_array[SPRITE_TYPE_MUSHROOM_WALK_RIGHT_1 + walk_index].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_MUSHROOM_RIGHT].width;
    }

    int cat_width = 26;
    int cat_gap = 6;
    for (int walk_index = 0; walk_index < 5; ++walk_index) {
        r_sprite_array[SPRITE_TYPE_CAT_WALK_RIGHT_1 + walk_index].height = 32;
        r_sprite_array[SPRITE_TYPE_CAT_WALK_RIGHT_1 + walk_index].width = cat_width;
        r_sprite_array[SPRITE_TYPE_CAT_WALK_RIGHT_1 + walk_index].flip_left_to_right = false;
        r_sprite_array[SPRITE_TYPE_CAT_WALK_RIGHT_1 + walk_index].pixels_start = image_array[IMAGE_INDEX_CAT_RIGHT].pixels + 2 + (cat_width + cat_gap) * walk_index;
        r_sprite_array[SPRITE_TYPE_CAT_WALK_RIGHT_1 + walk_index].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_CAT_RIGHT].width;
    }

    r_sprite_array[SPRITE_TYPE_GROUND_TEST].height = SPRITE_HEIGHT;
    r_sprite_array[SPRITE_TYPE_GROUND_TEST].width = SPRITE_WIDTH;
    r_sprite_array[SPRITE_TYPE_GROUND_TEST].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_GROUND_TEST].pixels_start = image_array[IMAGE_INDEX_SOLIDS_1].pixels;
    r_sprite_array[SPRITE_TYPE_GROUND_TEST].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_1].width;

    r_sprite_array[SPRITE_TYPE_GROUND_TEST2].height = SPRITE_HEIGHT;
    r_sprite_array[SPRITE_TYPE_GROUND_TEST2].width = SPRITE_WIDTH;
    r_sprite_array[SPRITE_TYPE_GROUND_TEST2].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_GROUND_TEST2].pixels_start = image_array[IMAGE_INDEX_SOLIDS_1].pixels + 48;
    r_sprite_array[SPRITE_TYPE_GROUND_TEST2].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_1].width;


}