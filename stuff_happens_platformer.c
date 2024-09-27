#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "stuff_happens.h"

#include "game_blocks.h"
#include "game_images.h"
#include "game_movement.h"
#include "game_sprites.h"
#include "game_state.h"
#include "graphics_constants.h"
#include "rendering.h"
#include "utils.c"

#define INITIAL_FRAMES_TO_WAIT 180

static long micros_per_frame;

// block grid 25 x 18 is 1200 x 864

static bool still_running;

// TODO Still used?
static uint32_t *blank_pixels;


// Pixel offset of the top-left corner of the area where blocks are rendered in the window
int blocks_area_offset_x;
int blocks_area_offset_y;

struct GameState game_state;

struct WorldRules world_rules;

struct InputState input_state;

// PRIVATE
inline struct Block* get_world_block_for_location(int x, int y) {
    // TODO is x or y outside the world bounds handled properly? Just force the world to have a rectangle of blocks all around the outmost edge?
    if (x < 0 || y < 0) {
        return NULL;
    }

    int block_x = x / SPRITE_WIDTH;
    if (block_x * SPRITE_WIDTH < x) {
        block_x += 1;
    }
    int block_y = y / SPRITE_HEIGHT;
    if (block_y * SPRITE_HEIGHT < y) {
        block_y += 1;
    }
    if (block_x >= WORLD_BLOCKS_WIDTH || block_y >= WORLD_BLOCKS_HEIGHT) {
        return NULL;
    }

    return &game_state.world_blocks[block_y * WORLD_BLOCKS_WIDTH + block_x];
}

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

    game_state.character.current_sprite = game_state.base_sprites[SPRITE_TYPE_ORC_STAND];
    game_state.character.center_x = WORLD_BLOCKS_WIDTH * SPRITE_WIDTH / 2;
    game_state.character.center_y_inverted = SPRITE_HEIGHT + game_state.base_sprites[SPRITE_TYPE_ORC_STAND].height / 2;
    game_state.character.width = game_state.character.current_sprite.width;
    game_state.character.height = game_state.character.current_sprite.height;
    game_state.character.x_velocity_pixels_per_second = 0;
    game_state.character.y_velocity_pixels_per_second = 0;
    game_state.character.motion = STOPPED_WALK_LEFT;

    printf("Initial character position is %f,%f\n", game_state.character.center_x, game_state.character.center_y_inverted);

    game_state.current_frame = -1;

    // TODO just for testing
    // for (int i = 0; i < WORLD_BLOCKS_HEIGHT * WORLD_BLOCKS_WIDTH; ++i) {
    //     printf("%d block type %u\n", i, game_state.world_blocks[i].type);
    // }
}

// PRIVATE
inline void initialize_world_rules(double frames_per_second) {
    world_rules.frames_per_second = frames_per_second;
    world_rules.microseconds_per_frame = (double)1000000/frames_per_second;
    // TODO remove all the per_frame values? Are they used anywhere?
    world_rules.gravity_pixels_per_second = 22;
    world_rules.gravity_pixels_per_frame = world_rules.gravity_pixels_per_second / frames_per_second;
    world_rules.x_movement_initial_speed_pixels_per_second = 40;
    world_rules.x_movement_initial_speed_pixels_per_frame = world_rules.x_movement_initial_speed_pixels_per_second / frames_per_second;
    world_rules.x_movement_next_speed_1_pixels_per_second = 80;
    world_rules.x_movement_next_speed_1_pixels_per_frame = world_rules.x_movement_next_speed_1_pixels_per_second / frames_per_second;

    world_rules.x_ground_acceleration_pixels_per_second = 400;
    world_rules.x_movement_max_speed_pixels_per_second = 1000;
}

// PRIVATE
inline void initialize_input_state() {
    input_state.left_button_press_frame = -1;
    input_state.right_button_press_frame = -1;
    input_state.up_button_press_frame = -1;
    input_state.down_button_press_frame = -1;
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
            // printf("Block at %d,%d will be drawn at %d,%d\n", block_x, block_y, top_left_x, top_left_y);
            write_sprite(top_left_x, top_left_y, game_state.world_blocks[WORLD_BLOCKS_WIDTH * block_y + block_x].sprite, width, r_pixels);

        }
    }

    int char_left = round(game_state.character.center_x - game_state.character.current_sprite.width/2) + blocks_area_offset_x;
    int char_top = blocks_area_offset_y + WORLD_BLOCKS_HEIGHT * SPRITE_HEIGHT - round(game_state.character.center_y_inverted + game_state.character.current_sprite.height/2);
    // printf("Draw orc at %d,%d\n", char_left, char_top);
    write_sprite_aliased(char_left, char_top, game_state.character.current_sprite, width, r_pixels);
}

// IMPLEMENTS
int initialize(int width, int height, long micros_per_frame_param) {
    printf("top of initialize\n");
    still_running = true;
    srand(time(NULL));
    micros_per_frame = micros_per_frame_param;
    double frames_per_second = (double)1000000/(double)micros_per_frame;

    initialize_world_rules(frames_per_second);

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

    initialize_input_state();

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
    game_state.current_frame = frame_count;


    handle_input(&game_state, &input_state, &world_rules, world_rules.microseconds_per_frame);
    printf("character.x_velocity_pixels_per_second is now %f\n", game_state.character.x_velocity_pixels_per_second);

    do_movement(&game_state.character, world_rules.microseconds_per_frame);
    printf("character.x is now %f\n", game_state.character.center_x);

    // printf("top of process_frame_and_blit\n");
    blit(pixels, width, height);

    // Try skipping processing for the first second or so since framerate skips often happen at the very start
    if (frame_count < INITIAL_FRAMES_TO_WAIT) {
        return;
    }
}

// IMPLEMENTS
void process_event(SDL_Event event) {
    printf("event type is %i\n", event.type);
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

}

// IMPLEMENTS
bool should_stop() {
    return !still_running;
}

// IMPLEMENTS
void cleanup() {
    free(blank_pixels);
}