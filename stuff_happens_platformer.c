#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "stuff_happens.h"

#include "graphics_constants.h"
#include "image_bmp_loader.h"
#include "utils.c"

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

#define SPRITE_WIDTH 48
#define SPRITE_HEIGHT 48

#define NUM_BMP_IMAGES 3
// "image" 0 is black pixels for empty space
#define IMAGE_PATH_SOLIDS_1 "/Users/matthew.enss/personal/c/assets/tilesets/FreeCuteTileset/Tileset.bmp"
//#define IMAGE_PATH_ORC_1 "/Users/matthew.enss/personal/c/assets/tilesets/Tiny RPG Character Asset Pack v1.02 -Free Soldier&Orc/Orc/Orc/Orc-pixels-corners.bmp"
#define IMAGE_PATH_ORC_1 "/Users/matthew.enss/personal/c/assets/tilesets/Tiny RPG Character Asset Pack v1.02 -Free Soldier&Orc/Orc/Orc/Orc.bmp"

#define IMAGE_INDEX_BLANK 0
#define IMAGE_INDEX_SOLIDS_1 1
#define IMAGE_INDEX_ORC_1 2
// #define
// #define

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
void load_images(struct ImageInfo* r_image_array) {
    r_image_array[IMAGE_INDEX_BLANK].pixels = (uint32_t*)malloc(SPRITE_WIDTH * SPRITE_HEIGHT * 4);
    for (int i = 0; i < SPRITE_WIDTH * SPRITE_HEIGHT; ++i) {
        r_image_array[IMAGE_INDEX_BLANK].pixels[i] = ALPHA;
    }
    r_image_array[IMAGE_INDEX_BLANK].height = SPRITE_HEIGHT;
    r_image_array[IMAGE_INDEX_BLANK].width = SPRITE_WIDTH;

    load_bmp_image(IMAGE_PATH_SOLIDS_1, r_image_array + IMAGE_INDEX_SOLIDS_1);
    flip_upside_down(r_image_array[IMAGE_INDEX_SOLIDS_1].pixels, r_image_array[IMAGE_INDEX_SOLIDS_1].width, r_image_array[IMAGE_INDEX_SOLIDS_1].height);
    load_bmp_image(IMAGE_PATH_ORC_1, r_image_array + IMAGE_INDEX_ORC_1);
    flip_upside_down(r_image_array[IMAGE_INDEX_ORC_1].pixels, r_image_array[IMAGE_INDEX_ORC_1].width, r_image_array[IMAGE_INDEX_ORC_1].height);
    printf("orc pic width=%d height=%d\n", r_image_array[IMAGE_INDEX_ORC_1].width, r_image_array[IMAGE_INDEX_ORC_1].height);
    for (int i = 0; i < r_image_array[IMAGE_INDEX_ORC_1].height * r_image_array[IMAGE_INDEX_ORC_1].width; ++i) {
        if (r_image_array[IMAGE_INDEX_ORC_1].pixels[i] != 0) {
            printf("orc pixel %d is %#010x\n", i, r_image_array[IMAGE_INDEX_ORC_1].pixels[i]);
        }
    }
}

struct Sprite {
    uint32_t* pixels_start;
    uint32_t height, width;
    uint32_t image_source_pitch_in_pixels;
};

#define NUM_SPRITE_TYPES 3
#define SPRITE_TYPE_EMPTY 0
#define SPRITE_TYPE_GROUND 1
#define SPRITE_TYPE_ORC_STAND 2

// PRIVATE
void initialize_sprites(struct ImageInfo* image_array, struct Sprite* r_sprite_array) {
    r_sprite_array[SPRITE_TYPE_EMPTY].height = SPRITE_HEIGHT;
    r_sprite_array[SPRITE_TYPE_EMPTY].width = SPRITE_WIDTH;
    r_sprite_array[SPRITE_TYPE_EMPTY].pixels_start = image_array[IMAGE_INDEX_BLANK].pixels;
    r_sprite_array[SPRITE_TYPE_EMPTY].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_BLANK].width;

    r_sprite_array[SPRITE_TYPE_GROUND].height = SPRITE_HEIGHT;
    r_sprite_array[SPRITE_TYPE_GROUND].width = SPRITE_WIDTH;
    r_sprite_array[SPRITE_TYPE_GROUND].pixels_start = image_array[IMAGE_INDEX_SOLIDS_1].pixels + 48 * image_array[IMAGE_INDEX_SOLIDS_1].width;
    r_sprite_array[SPRITE_TYPE_GROUND].image_source_pitch_in_pixels = image_array[IMAGE_INDEX_SOLIDS_1].width;

    // Upper left in the bmp is 60,83, bottom right is 81, 97
    // 44,542 to 65,556
    // height is 556-542=14
    // width  is 65-44 = 21
    struct ImageInfo larger_orcs;
    scale_image_up(image_array + IMAGE_INDEX_ORC_1, 2, &larger_orcs);

    r_sprite_array[SPRITE_TYPE_ORC_STAND].height = 28;
    r_sprite_array[SPRITE_TYPE_ORC_STAND].width = 42;
    r_sprite_array[SPRITE_TYPE_ORC_STAND].pixels_start = larger_orcs.pixels + (166 * larger_orcs.width) + 120;
    r_sprite_array[SPRITE_TYPE_ORC_STAND].image_source_pitch_in_pixels = larger_orcs.width;
}

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
    r_block_array[BLOCK_TYPE_EMPTY].type = BLOCK_TYPE_EMPTY;
    r_block_array[BLOCK_TYPE_EMPTY].effects_flags = 0;
    r_block_array[BLOCK_TYPE_EMPTY].sprite = sprite_array[SPRITE_TYPE_EMPTY];

    r_block_array[BLOCK_TYPE_GROUND].type = BLOCK_TYPE_GROUND;
    r_block_array[BLOCK_TYPE_GROUND].effects_flags = EFFECT_FLAG_SOLID;
    r_block_array[BLOCK_TYPE_GROUND].sprite = sprite_array[SPRITE_TYPE_GROUND];
}

#define WORLD_BLOCKS_WIDTH  25
#define WORLD_BLOCKS_HEIGHT 18
int blocks_area_offset_x;
int blocks_area_offset_y;

struct GameState{
    struct ImageInfo base_bmp_images[NUM_BMP_IMAGES];
    struct Sprite base_sprites[NUM_SPRITE_TYPES];
    struct Block base_blocks[NUM_BLOCK_TYPES];

    struct Block world_blocks[WORLD_BLOCKS_WIDTH * WORLD_BLOCKS_HEIGHT];
};

struct Character {
    double center_x;
    double center_y_inverted;
    double x_velocity_pixels_per_second;
    double y_velocity_pixels_per_second;
    struct Sprite current_sprite;
};

struct GameState game_state;
struct Character character;

// PRIVATE
void initialize_game_state() {
    load_images(game_state.base_bmp_images);
    initialize_sprites(game_state.base_bmp_images, game_state.base_sprites);
    initialize_blocks(game_state.base_sprites, game_state.base_blocks);

    for (int y = 0; y < WORLD_BLOCKS_HEIGHT; ++y) {
        for (int x = 0; x < WORLD_BLOCKS_WIDTH; ++x) {
            if (y == 0) {
                printf("world block at %d,%d is ground\n", x, y);
                game_state.world_blocks[WORLD_BLOCKS_WIDTH * y + x] = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else if (y == 1 && (x < 3 || x >= WORLD_BLOCKS_WIDTH - 3)) {
                printf("world block at %d,%d is ground\n", x, y);
                game_state.world_blocks[WORLD_BLOCKS_WIDTH * y + x] = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else if (y == 2 && (x < 1 || x >= WORLD_BLOCKS_WIDTH - 1)) {
                printf("world block at %d,%d is ground\n", x, y);
                game_state.world_blocks[WORLD_BLOCKS_WIDTH * y + x] = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else if (y == 3 && (x < 1 || x >= WORLD_BLOCKS_WIDTH - 1)) {
                printf("world block at %d,%d is ground\n", x, y);
                game_state.world_blocks[WORLD_BLOCKS_WIDTH * y + x] = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else {
                game_state.world_blocks[WORLD_BLOCKS_WIDTH * y + x] = game_state.base_blocks[BLOCK_TYPE_EMPTY];
            }
        }
    }

    character.center_x = WORLD_BLOCKS_WIDTH * SPRITE_WIDTH / 2;
    character.center_y_inverted = 1.5 * SPRITE_HEIGHT;
    character.x_velocity_pixels_per_second = 0;
    character.y_velocity_pixels_per_second = 0;

    printf("Initial character position is %f,%f\n", character.center_x, character.center_y_inverted);

    character.current_sprite = game_state.base_sprites[SPRITE_TYPE_ORC_STAND];

    // TODO just for testing
    // for (int i = 0; i < WORLD_BLOCKS_HEIGHT * WORLD_BLOCKS_WIDTH; ++i) {
    //     printf("%d block type %u\n", i, game_state.world_blocks[i].type);
    // }
}

// PRIVATE
inline void write_sprite(int top_left_x, int top_left_y, struct Sprite sprite, int pixels_width, uint32_t* r_pixels) {
    for (int y = 0; y < sprite.height; ++y) {
        int pixels_offset = (top_left_y + y) * pixels_width + top_left_x;
        // bmp images seem be loaded "upside-down", so invert the rows here
        // TODO remove this? int sprite_pixels_offset = (sprite.height - y - 1) * sprite.image_source_pitch_in_pixels;
        int sprite_pixels_offset = y * sprite.image_source_pitch_in_pixels;
        memcpy(r_pixels + pixels_offset, sprite.pixels_start + sprite_pixels_offset, sprite.width * 4);
    }
}

// PRIVATE
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

// PRIVATE
inline void blit(uint32_t* r_pixels, int width, int height) {
    memcpy(r_pixels, blank_pixels, width * height * 4);

    for (int block_y = 0; block_y < WORLD_BLOCKS_HEIGHT; ++block_y) {
        for (int block_x = 0; block_x < WORLD_BLOCKS_WIDTH; ++block_x) {
            // TODO not needed?
            // if (game_state.world_blocks[WORLD_BLOCKS_HEIGHT * block_y + block_x].type == BLOCK_TYPE_EMPTY) {
            //     continue;
            // }
            int top_left_x = blocks_area_offset_x + block_x * SPRITE_WIDTH;
            int top_left_y = blocks_area_offset_y + (WORLD_BLOCKS_HEIGHT - 1 - block_y) * SPRITE_HEIGHT;
            printf("Block at %d,%d will be drawn at %d,%d\n", block_x, block_y, top_left_x, top_left_y);
            write_sprite(top_left_x, top_left_y, game_state.world_blocks[WORLD_BLOCKS_WIDTH * block_y + block_x].sprite, width, r_pixels);

        }
    }

    int char_left = round(character.center_x - character.current_sprite.width/2) + blocks_area_offset_x;
    int char_top = blocks_area_offset_y + WORLD_BLOCKS_HEIGHT * SPRITE_HEIGHT - round(character.center_y_inverted + character.current_sprite.height/2);
    printf("Draw orc at %d,%d\n", char_left, char_top);
    write_sprite(char_left, char_top, character.current_sprite, width, r_pixels);
}

// IMPLEMENTS
int initialize(int width, int height, long micros_per_frame_param) {
    printf("top of initialize\n");
    still_running = true;
    srand(time(NULL));
    micros_per_frame = micros_per_frame_param;

    int blocks_area_width = SPRITE_WIDTH * WORLD_BLOCKS_WIDTH;
    int blocks_area_height = SPRITE_HEIGHT * WORLD_BLOCKS_HEIGHT;
    if (blocks_area_width >= width || blocks_area_height >= height) {
        printf("ERROR: window area too small for game display: need width=%d height=%d but got width=%d height=%d\n", blocks_area_width, blocks_area_height, width, height);
        return -1;
    }
    blocks_area_offset_x = (width - blocks_area_width) / 2;
    blocks_area_offset_y = (height - blocks_area_height) / 2;

    printf("blocks_area_width=%d blocks_area_height=%d blocks_area_offset_x=%d blocks_area_offset_y=%d\n", blocks_area_width, blocks_area_height, blocks_area_offset_x, blocks_area_offset_y);

    printf("About to initialize_game_state");
    initialize_game_state();
    printf("Done initialize_game_state");

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
    return 0;
}

// IMPLEMENTS
void process_frame_and_blit(long frame_count, long frame_time_in_micros, uint32_t *pixels, int width, int height) {
    // printf("top of process_frame_and_blit\n");
    blit(pixels, width, height);

    // Try skipping processing for the first second or so since framerate skips often happen at the very start
    if (frame_count < INITIAL_FRAMES_TO_WAIT) {
        return;
    }
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