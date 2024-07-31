#ifndef _IMAGE_PNG_LOADER__H
#define _IMAGE_PNG_LOADER__H

#include <inttypes.h>

struct Image_info {
    int width;
    int height;
    uint32_t* pixels;
};

// Returns error code, 0 is success
// Caller is responsible for deleting &pixels
// __FREE_REQUIRED for r_image_info.pixels
int load_image(const char* file_path, struct Image_info* r_image_info);

#endif // _IMAGE_PNG_LOADER__H