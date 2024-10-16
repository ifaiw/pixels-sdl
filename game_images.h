#ifndef _GAME_IMAGES__H
#define _GAME_IMAGES__H


#include "game_structs.h"

#define NUM_BMP_IMAGES 3
// "image" 0 is black pixels for empty space
#define IMAGE_PATH_SOLIDS_1 "/Users/matthew.enss/personal/c/assets/tilesets/FreeCuteTileset/Tileset.bmp"
//#define IMAGE_PATH_ORC_1 "/Users/matthew.enss/personal/c/assets/tilesets/Tiny RPG Character Asset Pack v1.02 -Free Soldier&Orc/Orc/Orc/Orc-pixels-corners.bmp"
#define IMAGE_PATH_ORC_1_RIGHT "/Users/matthew.enss/personal/c/assets/tilesets/Tiny RPG Character Asset Pack v1.02 -Free Soldier&Orc/Orc/Orc/Orc.bmp"

#define IMAGE_INDEX_BLANK 0
#define IMAGE_INDEX_SOLIDS_1 1
#define IMAGE_INDEX_ORC_1_RIGHT 2

void load_images(struct ImageInfo* r_image_array);

#endif  // _GAME_IMAGES__H