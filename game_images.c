#include "game_images.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "game_blocks.h"
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

// This modifies r_current_image.pixels directly
// All pixels that match old_pixel_colour ignoring alpha, are changed to new_pixel_colour, including the alpha value of new_pixel_colour
// PRIVATE
inline void swap_pixel_colour(struct ImageInfo* r_current_image, uint32_t old_pixel_colour, uint32_t new_pixel_colour) {
    old_pixel_colour = old_pixel_colour & NOT_ALPHA;
    for (int i = 0; i < r_current_image->width * r_current_image->height; ++i) {
        if ((r_current_image->pixels[i] & NOT_ALPHA) == old_pixel_colour) {
            r_current_image->pixels[i] = new_pixel_colour;
        }
    }
}

// IMPLEMENTS
int load_images(struct ImageInfo* r_image_array) {
    r_image_array[IMAGE_INDEX_BLANK].pixels = (uint32_t*)malloc(BLOCK_WIDTH_IN_PIXELS * BLOCK_HEIGHT_IN_PIXELS * 4);
    for (int i = 0; i < BLOCK_WIDTH_IN_PIXELS * BLOCK_HEIGHT_IN_PIXELS; ++i) {
        #ifdef CAT_CHARACTER
        // If using the black cat sprite, need to change the background to something other than black for the cat to show up
        r_image_array[IMAGE_INDEX_BLANK].pixels[i] = ALPHA | BLUE;
        #else
        r_image_array[IMAGE_INDEX_BLANK].pixels[i] = ALPHA;
        #endif
    }
    r_image_array[IMAGE_INDEX_BLANK].height = BLOCK_HEIGHT_IN_PIXELS;
    r_image_array[IMAGE_INDEX_BLANK].width = BLOCK_WIDTH_IN_PIXELS;

    int load_image_result = load_bmp_image(GAME_PATH__IMAGE_PATH_SOLIDS_1_FULL, r_image_array + IMAGE_INDEX_SOLIDS_1);
    if (load_image_result != 0) {
        printf("Error loading IMAGE_PATH_SOLIDS_1: %d\n", load_image_result);
        return load_image_result;
    }
    flip_upside_down(r_image_array[IMAGE_INDEX_SOLIDS_1].pixels, r_image_array[IMAGE_INDEX_SOLIDS_1].width, r_image_array[IMAGE_INDEX_SOLIDS_1].height);

    load_image_result = load_bmp_image(GAME_PATH__IMAGE_PATH_SOLIDS_2_FULL, r_image_array + IMAGE_INDEX_SOLIDS_2);
    if (load_image_result != 0) {
        printf("Error loading IMAGE_PATH_SOLIDS_2: %d\n", load_image_result);
        return load_image_result;
    }
    flip_upside_down(r_image_array[IMAGE_INDEX_SOLIDS_2].pixels, r_image_array[IMAGE_INDEX_SOLIDS_2].width, r_image_array[IMAGE_INDEX_SOLIDS_2].height);

    #ifdef CAT_CHARACTER
    // If using the black cat sprite, need to change the background to something other than black for the cat to show up
    swap_pixel_colour(&r_image_array[IMAGE_INDEX_SOLIDS_1], 0, ALPHA | BLUE);
    #endif
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

    struct ImageInfo mushrooms_image_unscaled;
    load_image_result = load_bmp_image(GAME_PATH__IMAGE_PATH_MUSHROOM_RIGHT_FULL, &mushrooms_image_unscaled);
    if (load_image_result != 0) {
        printf("Error loading GAME_PATH__IMAGE_PATH_MUSHROOM_RIGHT_FULL: %d\n", load_image_result);
        return load_image_result;
    }
    flip_upside_down(mushrooms_image_unscaled.pixels, mushrooms_image_unscaled.width, mushrooms_image_unscaled.height);
    scale_image_up(&mushrooms_image_unscaled, 2, r_image_array + IMAGE_INDEX_MUSHROOM_RIGHT);
    free(mushrooms_image_unscaled.pixels);

    load_image_result = load_bmp_image(GAME_PATH__IMAGE_PATH_CAT_RIGHT_FULL, r_image_array + IMAGE_INDEX_CAT_RIGHT);
    if (load_image_result != 0) {
        printf("Error loading GAME_PATH__IMAGE_PATH_CAT_RIGHT_FULL: %d\n", load_image_result);
        return load_image_result;
    }
    flip_upside_down(r_image_array[IMAGE_INDEX_CAT_RIGHT].pixels, r_image_array[IMAGE_INDEX_CAT_RIGHT].width, r_image_array[IMAGE_INDEX_CAT_RIGHT].height);

    return 0;
}