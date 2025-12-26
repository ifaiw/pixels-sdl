#ifndef _IMAGE_STRUCTS__H
#define _IMAGE_STRUCTS__H

#include <inttypes.h>

struct ImageInfo {
    int width;
    int height;
    uint32_t* pixels;
};

struct Sprite {
    uint32_t* pixels_start;
    uint32_t height, width;
    uint32_t image_source_pitch_in_pixels;
    // TODO only used for testing?
    int sprite_index;
};

#endif // _IMAGE_STRUCTS__H