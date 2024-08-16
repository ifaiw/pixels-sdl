#ifndef _IMAGE_BMP_LOADER__H
#define _IMAGE_BMP_LOADER__H

#include <inttypes.h>

#include "image_info.h"

// Returns error code, 0 is success
// Caller is responsible for deleting &pixels
// __FREE_REQUIRED for r_image_info.pixels
int load_bmp_image(const char* file_path, struct ImageInfo* r_image_info);

#endif // _IMAGE_BMP_LOADER__H