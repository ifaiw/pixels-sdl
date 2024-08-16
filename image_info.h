#ifndef _IMAGE_INFO__H
#define _IMAGE_INFO__H

#include <inttypes.h>

struct ImageInfo {
    int width;
    int height;
    uint32_t* pixels;
};

#endif  // _IMAGE_INFO__H