#include "game_sprites.h"

#include "game_images.h"

// IMPLEMENTS
void initialize_sprites(struct ImageInfo* image_array, struct Sprite* r_sprite_array) {
    r_sprite_array[SPRITE_TYPE_EMPTY].height = SPRITE_HEIGHT;
    r_sprite_array[SPRITE_TYPE_EMPTY].width = SPRITE_WIDTH;
    r_sprite_array[SPRITE_TYPE_EMPTY].pixels_start = image_array[IMAGE_INDEX_BLANK].pixels;
    r_sprite_array[SPRITE_TYPE_EMPTY].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_BLANK].width;

    r_sprite_array[SPRITE_TYPE_GROUND].height = SPRITE_HEIGHT;
    r_sprite_array[SPRITE_TYPE_GROUND].width = SPRITE_WIDTH;
    r_sprite_array[SPRITE_TYPE_GROUND].pixels_start = image_array[IMAGE_INDEX_SOLIDS_1].pixels + 48 * image_array[IMAGE_INDEX_SOLIDS_1].width;
    r_sprite_array[SPRITE_TYPE_GROUND].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_1].width;

    r_sprite_array[SPRITE_TYPE_ORC_STAND].height = 28;
    r_sprite_array[SPRITE_TYPE_ORC_STAND].width = 42;
    r_sprite_array[SPRITE_TYPE_ORC_STAND].pixels_start = image_array[IMAGE_INDEX_ORC_1].pixels + (166 * image_array[IMAGE_INDEX_ORC_1].width) + 120;
    r_sprite_array[SPRITE_TYPE_ORC_STAND].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_ORC_1].width;
}