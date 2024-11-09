#ifndef _GAME_IMAGES__H
#define _GAME_IMAGES__H


#include "game_structs.h"

#define NUM_BMP_IMAGES 3
// "image" 0 is black pixels for empty space

#define IMAGE_INDEX_BLANK 0
#define IMAGE_INDEX_SOLIDS_1 1
#define IMAGE_INDEX_ORC_1_RIGHT 2

// Non-zero results indicates error
int load_images(struct ImageInfo* r_image_array);

#endif  // _GAME_IMAGES__H