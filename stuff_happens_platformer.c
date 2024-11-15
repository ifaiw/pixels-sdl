#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "stuff_happens.h"

#include "game_blocks.h"
#include "game_images.h"
#include "game_movement.h"
#include "game_paths.h"
#include "game_sprites.h"
#include "game_state.h"
#include "graphics_constants.h"
#include "rendering.h"
#include "text_file_reader.h"
#include "utils.c"

#define INITIAL_FRAMES_TO_WAIT 180

static long micros_per_frame;

// block grid 25 x 18 is 1200 x 864

static bool still_running;

// TODO Still used?
static uint32_t *blank_pixels;

// TODO just for testing
int last_walking_frame;

struct GameState game_state;

struct InputState input_state;

struct CharacterSprite character_sprite;


// PRIVATE
void initialize_game_state() {
    printf("Top of initialize_game_state\n");
    fflush(stdout);
    printf("initialize_game_state: load_images\n");
    int load_image_result = load_images(game_state.base_bmp_images);
    if (load_image_result != 0) {
        printf("Error loading images: %d\n", load_image_result);
        exit(-1);
    }
    fflush(stdout);
    printf("initialize_game_state: initialize_sprites\n");
    initialize_sprites(game_state.base_bmp_images, game_state.base_sprites);
    printf("initialize_game_state: initialize_blocks\n");
    initialize_blocks(game_state.base_sprites, game_state.base_blocks);
    fflush(stdout);

    for (int y = 0; y < WORLD_BLOCKS_HEIGHT; ++y) {
        for (int x = 0; x < WORLD_BLOCKS_WIDTH; ++x) {
            if (y == 0) {
                printf("world block at %d,%d is ground\n", x, y);
                game_state.world_blocks[WORLD_BLOCKS_WIDTH * y + x] = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else if (y == 1 && (x < 1 || x >= WORLD_BLOCKS_WIDTH - 1)) {
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

            game_state.world_blocks[WORLD_BLOCKS_WIDTH * y + x].world_x = x;
            game_state.world_blocks[WORLD_BLOCKS_WIDTH * y + x].world_y = y;
            printf("Initializing blocks compare x %d to %d\n", game_state.base_blocks[BLOCK_TYPE_EMPTY].world_x, game_state.world_blocks[WORLD_BLOCKS_WIDTH * y + x].world_x);
        }
    }

    game_state.character_sprite.stand_sprite_index = SPRITE_TYPE_CAT_STAND_RIGHT;
    game_state.character_sprite.first_walk_sprite_index = SPRITE_TYPE_CAT_WALK_RIGHT_1;
    game_state.character_sprite.num_walking_animation_frames = 5;

    game_state.character.current_sprite = game_state.base_sprites[game_state.character_sprite.stand_sprite_index];
    game_state.character.x_bottom_left = WORLD_BLOCKS_WIDTH * BLOCK_WIDTH_IN_PIXELS / 2;
    game_state.character.y_inverted_bottom_left = BLOCK_HEIGHT_IN_PIXELS * 1;
    game_state.character.width = game_state.character.current_sprite.width;
    game_state.character.height = game_state.character.current_sprite.height;
    game_state.character.x_velocity_pixels_per_second = 0;
    game_state.character.y_velocity_pixels_per_second = 0;
    game_state.character.motion = STOPPED;
    game_state.character.direction = LEFT;

    printf("Initial character position is %f,%f\n", game_state.character.x_bottom_left, game_state.character.y_inverted_bottom_left);

    game_state.current_frame = -1;

    // for (int i = 0; i < WORLD_BLOCKS_HEIGHT * WORLD_BLOCKS_WIDTH; ++i) {
    //     printf("%d block type %u\n", i, game_state.world_blocks[i].type);
    // }
}



// PRIVATE
inline void initialize_world_rules(double frames_per_second, struct WorldRules* world_rules) {
    world_rules->frames_per_second = frames_per_second;
    world_rules->microseconds_per_frame = (double)1000000/frames_per_second;

    load_world_rules_from_file(world_rules);

    // printf("from dict_get_value, world_rules.gravity_pixels_per_second is %f\n", world_rules.gravity_pixels_per_second);
    // printf("from dict_get_value, world_rules.y_max_fall_speed_pixels_per_second is %f\n", world_rules.y_max_fall_speed_pixels_per_second);
    // printf("from dict_get_value, world_rules.x_ground_acceleration_pixels_per_second is %f\n", world_rules.x_ground_acceleration_pixels_per_second);
    // printf("from dict_get_value, world_rules.x_movement_max_speed_pixels_per_second is %f\n", world_rules.x_movement_max_speed_pixels_per_second);
    // printf("from dict_get_value, world_rules.micros_per_walking_animation_frame is %ld\n", world_rules.micros_per_walking_animation_frame);
    // printf("from dict_get_value, world_rules.y_jump_acceleration_pixels_per_second is %f\n", world_rules.y_jump_acceleration_pixels_per_second);
    // printf("from dict_get_value, world_rules.microseconds_after_jump_start_check_jump_still_pressed is %f\n", world_rules.microseconds_after_jump_start_check_jump_still_pressed);
}

// PRIVATE
inline void initialize_input_state() {
    input_state.left_button_press_frame = -1;
    input_state.right_button_press_frame = -1;
    input_state.up_button_press_frame = -1;
    input_state.down_button_press_frame = -1;
}

// PRIVATE
inline void update_sprites(struct GameState* game_state_param) {
    long microsecond_bucket;
    int walking_animation_frame_num;

    switch (game_state_param->character.motion) {
        case STOPPED:
            printf("motion is STOPPED show sprite STAND\n");
            game_state_param->character.current_sprite = game_state_param->base_sprites[game_state_param->character_sprite.stand_sprite_index];
            game_state_param->character.current_sprite.flip_left_to_right = game_state_param->character.direction == LEFT;
            break;
        case WALKING:
            microsecond_bucket = game_state_param->current_time_in_micros / game_state_param->world_rules.micros_per_walking_animation_frame;
            walking_animation_frame_num = microsecond_bucket % game_state_param->character_sprite.num_walking_animation_frames;
            // TODO just for testing
            if (walking_animation_frame_num != last_walking_frame && walking_animation_frame_num != last_walking_frame+1 && !(last_walking_frame == 6 && walking_animation_frame_num == 0)) {
                printf("skipped walk animation frame, from %d to %d\n", last_walking_frame, walking_animation_frame_num);
            }
            last_walking_frame = walking_animation_frame_num;

            printf("motion is WALKING show sprite WALK %d\n", walking_animation_frame_num);
            printf("we're walking, current_time_in_micros=%ld microsecond_bucket=%ld walking_animation_frame_num=%d\n", game_state_param->current_time_in_micros, microsecond_bucket, walking_animation_frame_num);
            game_state_param->character.current_sprite = game_state_param->base_sprites[game_state_param->character_sprite.first_walk_sprite_index + walking_animation_frame_num];
            game_state_param->character.current_sprite.flip_left_to_right = game_state_param->character.direction == LEFT;
            break;
        case JUMPING:
            printf("motion is STOPPED show sprite WALK0\n");
            game_state_param->character.current_sprite = game_state_param->base_sprites[game_state_param->character_sprite.first_walk_sprite_index];
            game_state_param->character.current_sprite.flip_left_to_right = game_state_param->character.direction == LEFT;
            break;
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
            int top_left_x = game_state.blocks_area_offset_x + block_x * BLOCK_WIDTH_IN_PIXELS;
            int top_left_y = game_state.blocks_area_offset_y + (WORLD_BLOCKS_HEIGHT - 1 - block_y) * BLOCK_HEIGHT_IN_PIXELS;
            // printf("Block at %d,%d will be drawn at %d,%d\n", block_x, block_y, top_left_x, top_left_y);
            write_sprite(top_left_x, top_left_y, game_state.world_blocks[WORLD_BLOCKS_WIDTH * block_y + block_x].sprite, width, r_pixels);

        }
    }

    int char_left = round(game_state.character.x_bottom_left) + game_state.blocks_area_offset_x;
    int char_top = game_state.blocks_area_offset_y + (WORLD_BLOCKS_HEIGHT * BLOCK_HEIGHT_IN_PIXELS) - round(game_state.character.y_inverted_bottom_left) - game_state.character.current_sprite.height;
    printf("Draw character at %d,%d\n", char_left, char_top);
    write_sprite_aliased(char_left, char_top, game_state.character.current_sprite, width, r_pixels);
}

// IMPLEMENTS
int initialize(int width, int height, long micros_per_frame_param) {
    printf("top of initialize\n");
    still_running = true;
    srand(time(NULL));
    micros_per_frame = micros_per_frame_param;
    double frames_per_second = (double)1000000/(double)micros_per_frame;

    initialize_world_rules(frames_per_second, &game_state.world_rules);

    int blocks_area_width = SPRITE_WIDTH * WORLD_BLOCKS_WIDTH;
    int blocks_area_height = SPRITE_HEIGHT * WORLD_BLOCKS_HEIGHT;
    if (blocks_area_width >= width || blocks_area_height >= height) {
        printf("ERROR: window area too small for game display: need width=%d height=%d but got width=%d height=%d\n", blocks_area_width, blocks_area_height, width, height);
        return -1;
    }
    game_state.blocks_area_offset_x = (width - blocks_area_width) / 2;
    game_state.blocks_area_offset_y = (height - blocks_area_height) / 2;

    printf("blocks_area_width=%d blocks_area_height=%d blocks_area_offset_x=%d blocks_area_offset_y=%d\n", blocks_area_width, blocks_area_height, game_state.blocks_area_offset_x, game_state.blocks_area_offset_y);

    printf("About to initialize_game_state\n");
    fflush(stdout);
    initialize_game_state();
    printf("Done initialize_game_state\n");

    initialize_input_state();

    // 60 frames per second
    // fps / 1000000 = frames per micros
    // 1000000/fps = micros per frame

    blank_pixels = (uint32_t*)malloc(height * width * 4);
    for (int i = 0; i < height * width; ++i) {
        blank_pixels[i] = ALPHA;
    }

    printf("bottom of initialize\n");
    return 0;
}

// IMPLEMENTS
void process_frame_and_blit(long frame_count, long current_time_in_micros, uint32_t *pixels, int width, int height) {
    game_state.current_frame = frame_count;
    game_state.current_time_in_micros = current_time_in_micros;

    game_state.character.is_on_ground = is_on_ground(&game_state);

    handle_input(&game_state, &input_state, game_state.world_rules.microseconds_per_frame);
    printf("character.x_velocity_pixels_per_second is now %f\n", game_state.character.x_velocity_pixels_per_second);

    do_movement(&game_state, game_state.world_rules.microseconds_per_frame);

    update_sprites(&game_state);

    printf("character.x is now %f\n", game_state.character.x_bottom_left);

    // printf("top of process_frame_and_blit\n");
    blit(pixels, width, height);
    // Below is test code for just rendering an image
    // struct Sprite test_sprite;
    // test_sprite.flip_left_to_right = false;
    // test_sprite.width = width;
    // test_sprite.height = height;
    // test_sprite.image_source_pitch_in_pixels = game_state.base_bmp_images[IMAGE_INDEX_MUSHROOM_1_RIGHT].width;
    // test_sprite.pixels_start = game_state.base_bmp_images[IMAGE_INDEX_MUSHROOM_1_RIGHT].pixels;
    // write_sprite(0, 0, test_sprite, width, pixels);
    // write_image(0, 0, game_state.base_bmp_images[IMAGE_INDEX_MUSHROOM_1_RIGHT], width, pixels);


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