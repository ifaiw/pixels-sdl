#include "game_images.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "game_paths.h"
#include "game_sprites.h"
#include "graphics_constants.h"
#include "image_bmp_loader.h"


// PRIVATE
inline void flip_upside_down(uint32_t* pixels, int width, int height) {
    int bottom = height - 1;
    uint32_t swap_row[width];
    for (int top = 0; top < bottom; ++top) {
        memcpy(swap_row, pixels + bottom * width, width * 4);
        memcpy(pixels + bottom * width, pixels + top * width, width * 4);
        memcpy(pixels + top * width, swap_row, width * 4);
        bottom--;
    }
}

// PRIVATE
inline void flip_right_left(uint32_t* pixels, int width, int height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width / 2; ++x) {
            uint32_t swap_pixel = pixels[y * width + x];
            pixels[y * width + x] = pixels[y * width + width - x - 1];
            pixels[y * width + width - x - 1] = swap_pixel;
        }
    }
}

// PRIVATE
inline void scale_image_up(struct ImageInfo* current_image, uint8_t factor_increase, struct ImageInfo* r_scaled_image) {
    r_scaled_image->height = current_image->height * factor_increase;
    r_scaled_image->width = current_image->width * factor_increase;
    r_scaled_image->pixels = (uint32_t*)malloc(r_scaled_image->height * r_scaled_image->width * 4);

    int scaled_y = 0;
    for (int y = 0; y < current_image->height; ++y) {
        int scaled_x = 0;
        for (int x = 0; x < current_image->width; ++x) {
            for (int i = 0; i < factor_increase; ++i) {
                r_scaled_image->pixels[scaled_y * r_scaled_image->width + scaled_x + i] = current_image->pixels[y * current_image->width + x];
            }
            scaled_x += factor_increase;
        }
        for (int i = 1; i < factor_increase; ++i) {
            memcpy( r_scaled_image->pixels + (scaled_y + i) * r_scaled_image->width,
                    r_scaled_image->pixels + scaled_y * r_scaled_image->width,
                    r_scaled_image->width * 4);
        }
        scaled_y += factor_increase;
    }
}

// IMPLEMENTS
int load_images(struct ImageInfo* r_image_array) {
    r_image_array[IMAGE_INDEX_BLANK].pixels = (uint32_t*)malloc(SPRITE_WIDTH * SPRITE_HEIGHT * 4);
    for (int i = 0; i < SPRITE_WIDTH * SPRITE_HEIGHT; ++i) {
        r_image_array[IMAGE_INDEX_BLANK].pixels[i] = ALPHA;
    }
    r_image_array[IMAGE_INDEX_BLANK].height = SPRITE_HEIGHT;
    r_image_array[IMAGE_INDEX_BLANK].width = SPRITE_WIDTH;

    int load_image_result = load_bmp_image(GAME_PATH__IMAGE_PATH_SOLIDS_1_FULL, r_image_array + IMAGE_INDEX_SOLIDS_1);
    if (load_image_result != 0) {
        printf("Error loading IMAGE_PATH_SOLIDS_1: %d\n", load_image_result);
        return load_image_result;
    }
    flip_upside_down(r_image_array[IMAGE_INDEX_SOLIDS_1].pixels, r_image_array[IMAGE_INDEX_SOLIDS_1].width, r_image_array[IMAGE_INDEX_SOLIDS_1].height);
    // TODO not needed? flip_right_left(r_image_array[IMAGE_INDEX_SOLIDS_1].pixels, r_image_array[IMAGE_INDEX_SOLIDS_1].width, r_image_array[IMAGE_INDEX_SOLIDS_1].height);

    struct ImageInfo orcs_image_unscaled;
    load_image_result = load_bmp_image(GAME_PATH__IMAGE_PATH_ORC_1_RIGHT_FULL, &orcs_image_unscaled);
    if (load_image_result != 0) {
        printf("Error loading IMAGE_PATH_ORC_1_RIGHT: %d\n", load_image_result);
        return load_image_result;
    }
    flip_upside_down(orcs_image_unscaled.pixels, orcs_image_unscaled.width, orcs_image_unscaled.height);
    // TODO not needed? flip_right_left(orcs_image_unscaled.pixels, orcs_image_unscaled.width, orcs_image_unscaled.height);
    scale_image_up(&orcs_image_unscaled, 2, r_image_array + IMAGE_INDEX_ORC_1_RIGHT);
    free(orcs_image_unscaled.pixels);

    // TODO can get rid of?
    // r_image_array[IMAGE_INDEX_ORC_1_LEFT].height = r_image_array[IMAGE_INDEX_ORC_1_RIGHT].height;
    // r_image_array[IMAGE_INDEX_ORC_1_LEFT].width = r_image_array[IMAGE_INDEX_ORC_1_RIGHT].width;
    // r_image_array[IMAGE_INDEX_ORC_1_LEFT].pixels = (uint32_t*)malloc(sizeof(r_image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels));
    // memcpy(r_image_array[IMAGE_INDEX_ORC_1_LEFT].pixels, r_image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels, sizeof(r_image_array[IMAGE_INDEX_ORC_1_RIGHT].pixels));
    // flip_right_left(r_image_array[IMAGE_INDEX_ORC_1_LEFT].pixels, r_image_array[IMAGE_INDEX_ORC_1_LEFT].width, r_image_array[IMAGE_INDEX_ORC_1_LEFT].height);

    return 0;
}