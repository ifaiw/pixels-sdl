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

    r_sprite_array[SPRITE_TYPE_ORC_STAND_RIGHT].height = 28;
    r_sprite_array[SPRITE_TYPE_ORC_STAND_RIGHT].width = 42;
    r_sprite_array[SPRITE_TYPE_ORC_STAND_RIGHT].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_ORC_STAND_RIGHT].pixels_start = image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels + (166 * image_array[IMAGE_INDEX_ORC_1_RIGHT].width) + 120;
    r_sprite_array[SPRITE_TYPE_ORC_STAND_RIGHT].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_ORC_1_RIGHT].width;

    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_1].height = 28;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_1].width = 42;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_1].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_1].pixels_start = image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels + (366 * image_array[IMAGE_INDEX_ORC_1_RIGHT].width) + 120;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_1].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_ORC_1_RIGHT].width;

    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_2].height = 28;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_2].width = 42;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_2].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_2].pixels_start = image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels + (366 * image_array[IMAGE_INDEX_ORC_1_RIGHT].width) + 320;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_2].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_ORC_1_RIGHT].width;

    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_3].height = 28;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_3].width = 42;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_3].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_3].pixels_start = image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels + (366 * image_array[IMAGE_INDEX_ORC_1_RIGHT].width) + 520;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_3].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_ORC_1_RIGHT].width;

    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_4].height = 28;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_4].width = 42;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_4].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_4].pixels_start = image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels + (366 * image_array[IMAGE_INDEX_ORC_1_RIGHT].width) + 720;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_4].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_ORC_1_RIGHT].width;

    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_5].height = 28;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_5].width = 42;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_5].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_5].pixels_start = image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels + (366 * image_array[IMAGE_INDEX_ORC_1_RIGHT].width) + 920;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_5].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_ORC_1_RIGHT].width;

    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_6].height = 28;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_6].width = 42;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_6].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_6].pixels_start = image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels + (366 * image_array[IMAGE_INDEX_ORC_1_RIGHT].width) + 1120;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_6].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_ORC_1_RIGHT].width;

    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_7].height = 28;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_7].width = 42;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_7].flip_left_to_right = false;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_7].pixels_start = image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels + (366 * image_array[IMAGE_INDEX_ORC_1_RIGHT].width) + 1320;
    r_sprite_array[SPRITE_TYPE_ORC_WALK_RIGHT_7].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_ORC_1_RIGHT].width;


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