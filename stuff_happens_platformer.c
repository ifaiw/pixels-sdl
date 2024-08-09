#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "stuff_happens.h"

#include "graphics_constants.h"
#include "image_bmp_loader.h"

#define INITIAL_FRAMES_TO_WAIT 180

static long micros_per_frame;

// block grid 25 x 18 is 1200 x 864

// TODO All still used?
static double gravity_pixels_per_frame;
static double gravity_pixels_per_second = 22;
static double vertical_resistance_factor = 0.992;
static double horizontal_friction_factor = 0.96;

static bool still_running;

// TODO Still used?
static uint32_t *blank_pixels;

#define NUM_BMP_IMAGES 2
#define IMAGE_PATH_SOLIDS_1 "/Users/matthew.enss/personal/c/assets/tilesets/FreeCuteTileset/Tileset.bmp"
#define IMAGE_INDEX_SOLIDS_1 0
// #define
// #define

// PRIVATE
void load_images(struct ImageInfo* r_image_array) {
    load_bmp_image(IMAGE_PATH_SOLIDS_1, r_image_array + IMAGE_INDEX_SOLIDS_1);
}

struct Sprite {
    uint32_t* pixels_start;
    uint32_t height, width;
    uint32_t pitch_in_pixels;
};

#define NUM_SPRITE_TYPES 2
#define SPRITE_TYPE_EMPTY 0
#define SPRITE_TYPE_GROUND 1

// PRIVATE
void initialize_sprites( struct Sprite* r_sprite_array) {


}

struct Character {
    double radius;
    double x;
    double y;
    uint32_t colour;
    double thickness;
    double x_speed;
    double y_speed;
};

#define NUM_BLOCK_TYPES 2
#define BLOCK_TYPE_EMPTY 0
#define BLOCK_TYPE_GROUND 1

#define EFFECT_FLAG_SOLID 0x00000001

struct Block {
    uint16_t type;
    uint32_t effects_flags;
    struct Sprite sprite;
};

// PRIVATE
void initialize_blocks(struct Sprite* sprite_array, struct Block* r_block_array) {
    r_block_array[0].type = BLOCK_TYPE_EMPTY;
    r_block_array[0].effects_flags = 0;

}

// PRIVATE
inline void blit(uint32_t *pixels, int width, int height) {
    memcpy(pixels, blank_pixels, width * height * 4);


}

// IMPLEMENTS
void initialize(int width, int height, long micros_per_frame_param) {
    printf("top of initialize\n");
    still_running = true;
    srand(time(NULL));
    micros_per_frame = micros_per_frame_param;

    gravity_pixels_per_frame = (double)gravity_pixels_per_second / (double)1000000 * (double)micros_per_frame;
    // 60 frames per second
    // fps / 1000000 = frames per micros
    // 1000000/fps = micros per frame
    // gravity per second / 1000000 = gravity per micros
    // gravity per micros * micros per frame = gravity per frame

    blank_pixels = (uint32_t*)malloc(height * width * 4);
    for (int i = 0; i < height * width; ++i) {
        blank_pixels[i] = ALPHA;
    }

    printf("bottom of initialize\n");
}

// IMPLEMENTS
void process_frame_and_blit(long frame_count, long frame_time_in_micros, uint32_t *pixels, int width, int height) {
    printf("top of process_frame_and_blit\n");
    blit(pixels, width, height);

    // Try skipping processing for the first second or so since framerate skips often happen at the very start
    if (frame_count > INITIAL_FRAMES_TO_WAIT) {

    } // if to skip processing on initial frames
}

// IMPLEMENTS
void process_event(SDL_Event event) {
    // printf("event type is %i\n", event.type);
    // Handle each specific event
    if(event.type == SDL_QUIT){
        still_running = false;
    }
    // Handle Mouse motion
    if(event.type == SDL_MOUSEMOTION){
    }
    // Handle key presses
    if(event.type == SDL_KEYDOWN){
    }
    // Handle specific key presses from scancodes
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_RIGHT]){
        printf("right arrow key is pressed\n");
    }
    else if (state[SDL_SCANCODE_LEFT]){
        printf("left arrow key is pressed\n");
    }
    if(state[SDL_SCANCODE_UP]){
        printf("up arrow key is pressed\n");

        // gravity_pixels_per_second += 1;
        // gravity_pixels_per_frame = (double)gravity_pixels_per_second / (double)1000000 * (double)micros_per_frame;
    }
    else if (state[SDL_SCANCODE_DOWN]){
        printf("down arrow key is pressed, ignored for now\n");
        // gravity_pixels_per_second -= 1;
        // gravity_pixels_per_frame = (double)gravity_pixels_per_second / (double)1000000 * (double)micros_per_frame;
    }
}

// IMPLEMENTS
bool should_stop() {
    return !still_running;
}

// IMPLEMENTS
void cleanup() {
    free(blank_pixels);
}