#ifndef _RENDERING__H
#define _RENDERING__H

#include <inttypes.h>
#include <math.h>
#include <string.h>

#include "game_structs.h"
#include "graphics_constants.h"

inline void write_sprite(int top_left_x, int top_left_y, struct Sprite sprite, int pixels_width, uint32_t* r_pixels) {
    for (int y = 0; y < sprite.height; ++y) {
        int pixels_offset = (top_left_y + y) * pixels_width + top_left_x;
        // bmp images seem be loaded "upside-down", so invert the rows here
        // TODO remove this? int sprite_pixels_offset = (sprite.height - y - 1) * sprite.image_source_pitch_in_pixels;
        int sprite_pixels_offset = y * sprite.image_source_pitch_in_pixels;
        memcpy(r_pixels + pixels_offset, sprite.pixels_start + sprite_pixels_offset, sprite.width * 4);
    }
}

inline void write_sprite_aliased(int top_left_x, int top_left_y, struct Sprite sprite, int pixels_width, uint32_t* r_pixels) {
    for (int y = 0; y < sprite.height; ++y) {
        uint32_t* sprite_row_start = sprite.pixels_start + (y * sprite.image_source_pitch_in_pixels);
        uint32_t* pixels_row_start = r_pixels + ((top_left_y + y) * pixels_width) + top_left_x;
        for (int pixel_index = 0; pixel_index < sprite.width; ++pixel_index) {
            switch (sprite_row_start[pixel_index] & ALPHA) {
                case ALPHA: pixels_row_start[pixel_index] = sprite_row_start[pixel_index];
                default: break;
            }
        }
        // TODO Remove all of these commented lines?
        // // bmp images seem be loaded "upside-down", so invert the rows here
        // // TODO remove this? int sprite_pixels_offset = (sprite.height - y - 1) * sprite.image_source_pitch_in_pixels;
        // int sprite_pixels_offset = y * sprite.image_source_pitch_in_pixels;
        // memcpy(r_pixels + pixels_offset, sprite.pixels_start + sprite_pixels_offset, sprite.width * 4);
    }
}

inline void draw_circle(
    uint32_t *pixels,
    int pixels_pitch_in_pixels,
    int circle_x,
    int circle_y,
    double radius,
    uint32_t colour)
{
    // if (circle_x - radius < 0 || circle_x + radius >= window_width || circle_y - radius < 0 || circle_y + radius >= window_height) {
    //     // printf("Illegal circle! %d %d %f", circle_x, circle_y, radius);
    //     return;
    // }

    int x = 0;
    int y = round(sqrt((double)(radius * radius - (double)(x * x))));
    while (x <= y) {
        pixels[x + circle_x + ((y + circle_y) * pixels_pitch_in_pixels)] = colour;
        pixels[x + circle_x + ((-y + circle_y) * pixels_pitch_in_pixels)] = colour;
        pixels[-x + circle_x + ((y + circle_y) * pixels_pitch_in_pixels)] = colour;
        pixels[-x + circle_x + ((-y + circle_y) * pixels_pitch_in_pixels)] = colour;

        pixels[y + circle_x + ((x + circle_y) * pixels_pitch_in_pixels)] = colour;
        pixels[y + circle_x + ((-x + circle_y) * pixels_pitch_in_pixels)] = colour;
        pixels[-y + circle_x + ((x + circle_y) * pixels_pitch_in_pixels)] = colour;
        pixels[-y + circle_x + ((-x + circle_y) * pixels_pitch_in_pixels)] = colour;

        x += 1;
        y = round(sqrt((double)(radius * radius - (double)(x * x))));
    }
}

#endif  // _RENDERING__H