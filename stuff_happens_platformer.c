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
#include "file_stuff.h"
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
struct ViewState view_state;
struct InputState input_state;
struct EditorState editor_state;

struct CharacterSprite character_sprite;


// PRIVATE
void initialize_game_state() {
    // printf("Top of initialize_game_state\n");
    fflush(stdout);
    // printf("initialize_game_state: load_images\n");
    int load_image_result = load_images(game_state.base_bmp_images);
    if (load_image_result != 0) {
        // printf("Error loading images: %d\n", load_image_result);
        exit(-1);
    }
    fflush(stdout);
    // printf("initialize_game_state: initialize_sprites\n");
    initialize_sprites(game_state.base_bmp_images, game_state.base_sprites);
    // printf("initialize_game_state: initialize_blocks\n");
    initialize_blocks(game_state.base_sprites, game_state.base_blocks);
    fflush(stdout);

    struct Block base_block_for_spot;
    for (int y = 0; y < HEIGHT_OF_WORLD_IN_BLOCKS; ++y) {
        for (int x = 0; x < WIDTH_OF_WORLD_IN_BLOCKS; ++x) {
            if (y < 6) {
                // printf("world block at %d,%d is ground\n", x, y);
                base_block_for_spot = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else if (y == 6 && (x < 1 || x >= WIDTH_OF_WORLD_IN_BLOCKS - 1)) {
                // printf("world block at %d,%d is ground\n", x, y);
                base_block_for_spot = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else if (y == 7 && (x % 5 == 0)) {
                // printf("world block at %d,%d is ground\n", x, y);
                base_block_for_spot = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else if (y == 8 && (x % 3 == 0)) {
                // printf("world block at %d,%d is ground\n", x, y);
                base_block_for_spot = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else if (x == 0) {
                // printf("world block at %d,%d is ground\n", x, y);
                base_block_for_spot = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else if (x >= WIDTH_OF_SCREEN_IN_BLOCKS) {
                // printf("world block at %d,%d is ground\n", x, y);
                base_block_for_spot = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else if (y >= 13) {
                // printf("world block at %d,%d is ground\n", x, y);
                base_block_for_spot = game_state.base_blocks[BLOCK_TYPE_GROUND];
            } else {
                base_block_for_spot = game_state.base_blocks[BLOCK_TYPE_EMPTY];
            }
            // printf("set block at %d,%d\n", x, y);
            fflush(stdout);
            game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * y + x] = base_block_for_spot;

            game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * y + x].world_pixel_x = x * BLOCK_WIDTH_IN_PIXELS;
            game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * y + x].world_pixel_y = y * BLOCK_HEIGHT_IN_PIXELS;
            game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * y + x].block_x = x;
            game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * y + x].block_y = y;
            // printf("Initialize world block x=%d y=%d world_pixel_x=%d world_pixel_y=%d\n", x, y, game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * y + x].world_pixel_x, game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * y + x].world_pixel_y);
        }
    }

    update_ground_images(&game_state);

    #ifdef CAT_CHARACTER // TODO no longer works now that I added climbing?
    game_state.character_sprite.stand_sprite_index = SPRITE_TYPE_CAT_STAND_RIGHT;
    game_state.character_sprite.first_walk_sprite_index = SPRITE_TYPE_CAT_WALK_RIGHT_1;
    game_state.character_sprite.num_walking_animation_frames = 5;
    #else
    game_state.character_sprite.stand_sprite_index = SPRITE_TYPE_MUSHROOM_STAND_RIGHT;
    game_state.character_sprite.first_walk_sprite_index = SPRITE_TYPE_MUSHROOM_WALK_RIGHT_1;
    game_state.character_sprite.num_walking_animation_frames = 7;
    game_state.character_sprite.first_climb_sprite_index = SPRITE_TYPE_MUSHROOM_CLIMB_1;
    game_state.character_sprite.num_climbing_animation_frames = 3;
    #endif

    game_state.character.current_sprite = game_state.base_sprites[game_state.character_sprite.stand_sprite_index];
    game_state.character.x_bottom_left = 680;
    game_state.character.y_inverted_bottom_left = BLOCK_HEIGHT_IN_PIXELS * 7;
    game_state.character.width = game_state.character.current_sprite.width;
    game_state.character.height = game_state.character.current_sprite.height;
    game_state.character.x_velocity_pixels_per_second = 0;
    game_state.character.y_velocity_pixels_per_second = 0;
    game_state.character.motion = STOPPED;
    game_state.character.direction = LEFT;

    // printf("Initial character position is %f,%f\n", game_state.character.x_bottom_left, game_state.character.y_inverted_bottom_left);

    game_state.current_frame = -1;

    // for (int i = 0; i < WORLD_BLOCKS_HEIGHT * WORLD_BLOCKS_WIDTH; ++i) {
    //     // printf("%d block type %u\n", i, game_state.world_blocks[i].type);
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

    input_state.letter_keys_down_bitmask = 0;
    input_state.number_keys_down_bitmask = 0;
    input_state.mouse_button_state = 0;
}

// PRIVATE
inline void update_sprites(struct GameState* game_state_param, struct InputState* input_state_param) {
    long microsecond_bucket;
    int walking_animation_frame_num;
    double pixel_distance_bucket;
    int climbing_animation_frame_num;

    switch (game_state_param->character.motion) {
        case STOPPED:
            // printf("motion is STOPPED show sprite STAND\n");
            game_state_param->character.current_sprite = game_state_param->base_sprites[game_state_param->character_sprite.stand_sprite_index];
            break;
        case CLIMBING:
            // Do 2 times y distance to make climbing up/down make the character animation move faster
            printf("climbing frame pixels_per_climbing_animation_frame=%f y_inverted_bottom_left=%f start_climb_pixel_y=%f x_bottom_left=%f start_climb_pixel_x=%f\n", game_state_param->world_rules.pixels_per_climbing_animation_frame, game_state_param->character.y_inverted_bottom_left, input_state_param->start_climb_pixel_y, game_state_param->character.x_bottom_left, input_state_param->start_climb_pixel_x);
            pixel_distance_bucket = (2 * abs(game_state_param->character.y_inverted_bottom_left - input_state_param->start_climb_pixel_y) + abs(game_state_param->character.x_bottom_left - input_state_param->start_climb_pixel_x)) / game_state_param->world_rules.pixels_per_climbing_animation_frame;

            // TODO casting to int here seems fishy
            climbing_animation_frame_num = (int)round(pixel_distance_bucket) % game_state_param->character_sprite.num_climbing_animation_frames;
            printf("climbing frame pixel_distance_bucket=%f climbing_animation_frame_num=%d\n", pixel_distance_bucket, climbing_animation_frame_num);
            game_state_param->character.current_sprite = game_state_param->base_sprites[game_state_param->character_sprite.first_climb_sprite_index + climbing_animation_frame_num];
            break;
        case WALKING:
            microsecond_bucket = game_state_param->current_time_in_micros / game_state_param->world_rules.micros_per_walking_animation_frame;
            walking_animation_frame_num = microsecond_bucket % game_state_param->character_sprite.num_walking_animation_frames;
            // TODO just for testing
            if (walking_animation_frame_num != last_walking_frame && walking_animation_frame_num != last_walking_frame+1 && !(last_walking_frame == 6 && walking_animation_frame_num == 0)) {
                // printf("skipped walk animation frame, from %d to %d\n", last_walking_frame, walking_animation_frame_num);
            }
            last_walking_frame = walking_animation_frame_num;

            // printf("motion is WALKING show sprite WALK %d\n", walking_animation_frame_num);
            // printf("we're walking, current_time_in_micros=%ld microsecond_bucket=%ld walking_animation_frame_num=%d\n", game_state_param->current_time_in_micros, microsecond_bucket, walking_animation_frame_num);
            game_state_param->character.current_sprite = game_state_param->base_sprites[game_state_param->character_sprite.first_walk_sprite_index + walking_animation_frame_num];
            break;
        case JUMPING:
            // printf("motion is STOPPED show sprite WALK0\n");
            game_state_param->character.current_sprite = game_state_param->base_sprites[game_state_param->character_sprite.first_walk_sprite_index];
            break;
    }
}

// IMPLEMENTS
int initialize(int width, int height, long micros_per_frame_param) {
    // printf("top of initialize\n");
    still_running = true;
    srand(time(NULL));
    micros_per_frame = micros_per_frame_param;
    double frames_per_second = (double)1000000/(double)micros_per_frame;

    initialize_world_rules(frames_per_second, &game_state.world_rules);

    int blocks_area_width = BLOCK_WIDTH_IN_PIXELS * WIDTH_OF_SCREEN_IN_BLOCKS;
    int blocks_area_height = BLOCK_HEIGHT_IN_PIXELS * HEIGHT_OF_SCREEN_IN_BLOCKS;
    if (blocks_area_width >= width || blocks_area_height >= height) {
        // printf("ERROR: window area too small for game display: need width=%d height=%d but got width=%d height=%d\n", blocks_area_width, blocks_area_height, width, height);
        return -1;
    }
    view_state.window_width = width;
    view_state.window_height = height;
    view_state.view_width = blocks_area_width;
    view_state.view_height = blocks_area_height;
    view_state.view_area_offset_x = (width - blocks_area_width) / 2;
    view_state.view_area_offset_y = (height - blocks_area_height) / 2;

    // printf("About to initialize_game_state\n");
    fflush(stdout);
    initialize_game_state();
    // printf("Done initialize_game_state\n");

    //mattt TODO just for testing
    save_level_to_disk(&game_state, GAME_PATH__LEVEL_FILES_FULL);
    exit(0);

    initialize_input_state();

    // 60 frames per second
    // fps / 1000000 = frames per micros
    // 1000000/fps = micros per frame

    blank_pixels = (uint32_t*)malloc(height * width * 4);
    for (int i = 0; i < height * width; ++i) {
        blank_pixels[i] = ALPHA;
    }

    editor_state.block_type = BLOCK_TYPE_LADDER;

    // printf("bottom of initialize\n");
    return 0;
}

// PRIVATE
inline void blit(uint32_t* r_pixels, int width, int height) {
    memcpy(r_pixels, blank_pixels, width * height * 4);

    view_state.view_bottom_left_world_x = game_state.character.x_bottom_left + (game_state.character.width / 2) - (view_state.view_width / 2);
    view_state.view_bottom_left_world_y = game_state.character.y_inverted_bottom_left + (game_state.character.height / 2) - (view_state.view_height / 2);
    int view_top_right_world_x = view_state.view_bottom_left_world_x + view_state.view_width;
    int view_top_right_world_y = view_state.view_bottom_left_world_y + view_state.view_height;

    // TODO can delete?
    // struct Block* bottom_left_block = get_world_block_for_location(view_state.view_bottom_left_world_x, view_state.view_bottom_left_world_y, &game_state);
    // struct Block* top_right_block = get_world_block_for_location(view_top_right_world_x, view_top_right_world_y, &game_state);

    int left_block_x;
    if (view_state.view_bottom_left_world_x < 0) {
        left_block_x = (view_state.view_bottom_left_world_x / BLOCK_WIDTH_IN_PIXELS) - 1;
    } else {
        left_block_x = view_state.view_bottom_left_world_x / BLOCK_WIDTH_IN_PIXELS;
    }
    int left_block_x_left_pixel_x = left_block_x * BLOCK_WIDTH_IN_PIXELS;
    int right_block_x;
    if (view_top_right_world_x < 0) {
        right_block_x = (view_top_right_world_x / BLOCK_WIDTH_IN_PIXELS) - 1;
    } else {
        right_block_x = view_top_right_world_x / BLOCK_WIDTH_IN_PIXELS;
    }
    int right_block_x_left_pixel_x = right_block_x * BLOCK_WIDTH_IN_PIXELS;

    int bottom_block_y;
    if (view_state.view_bottom_left_world_y < 0) {
        bottom_block_y = (view_state.view_bottom_left_world_y / BLOCK_HEIGHT_IN_PIXELS) - 1;
    } else {
        bottom_block_y = view_state.view_bottom_left_world_y / BLOCK_HEIGHT_IN_PIXELS;
    }
    int bottom_block_y_bottom_pixel_y = bottom_block_y * BLOCK_HEIGHT_IN_PIXELS;
    int top_block_y;
    if (view_top_right_world_y < 0) {
        top_block_y = (view_top_right_world_y / BLOCK_HEIGHT_IN_PIXELS) - 1;
    } else {
        top_block_y = view_top_right_world_y / BLOCK_HEIGHT_IN_PIXELS;
    }
    int top_block_y_bottom_pixel_y = top_block_y * BLOCK_HEIGHT_IN_PIXELS;

    // printf("Calculatic top_block_num_y_rows_visible using view_state.view_bottom_left_world_y=%d view_state.view_height=%d top_block_y_bottom_pixel_y=%d\n", view_state.view_bottom_left_world_y, view_state.view_height, top_block_y_bottom_pixel_y);
    int top_block_num_y_rows_visible = view_state.view_bottom_left_world_y + view_state.view_height - top_block_y_bottom_pixel_y;
    int bottom_block_num_y_rows_visible = bottom_block_y_bottom_pixel_y + BLOCK_HEIGHT_IN_PIXELS - view_state.view_bottom_left_world_y;

    int left_block_num_x_cols_visible = left_block_x_left_pixel_x + BLOCK_WIDTH_IN_PIXELS - view_state.view_bottom_left_world_x;
    int right_block_num_x_cols_visible = view_state.view_bottom_left_world_x + view_state.view_width - right_block_x_left_pixel_x;

    // TODO clean up all the block drawing code below; different parts use different ways to reference the correct pixel on the screen
    // printf("blitting blocks in view\n");

    // Draw the full blocks in the middle
    for (int block_y_index = bottom_block_y + 1; block_y_index < top_block_y; ++block_y_index) {
        // printf("calculate top_left_y using view_state.view_area_offset_y=%d top_block_num_y_rows_visible=%d block_y_index=%d bottom_block_y=%d BLOCK_HEIGHT_IN_PIXELS=%d\n", view_state.view_area_offset_y, top_block_num_y_rows_visible, block_y_index, bottom_block_y, BLOCK_HEIGHT_IN_PIXELS);
        int top_left_y = view_state.view_area_offset_y + top_block_num_y_rows_visible + ((top_block_y - block_y_index - 1) * BLOCK_HEIGHT_IN_PIXELS);
        for (int block_x_index = left_block_x + 1; block_x_index < right_block_x; ++block_x_index) {
            if (block_x_index < 0 || block_y_index < 0 || block_x_index >= WIDTH_OF_WORLD_IN_BLOCKS || block_y_index >= HEIGHT_OF_WORLD_IN_BLOCKS) {
                // printf("Skipping block for %d,%d because it doesn't exist\n", block_x_index, block_y_index);
                continue;
            }
            // printf("top left for block calculated using view_state.view_area_offset_x=%d left_block_num_x_cols_visible=%d block_x_index=%d left_block_x=%d BLOCK_WIDTH_IN_PIXELS=%d\n", view_state.view_area_offset_x, left_block_num_x_cols_visible, block_x_index, left_block_x, BLOCK_WIDTH_IN_PIXELS);
            int top_left_x = view_state.view_area_offset_x + left_block_num_x_cols_visible + ((block_x_index - left_block_x - 1) * BLOCK_WIDTH_IN_PIXELS);
            // TODO just for testing
            int block_index = WIDTH_OF_WORLD_IN_BLOCKS * block_y_index + block_x_index;
            int top_left_test_x = game_state.world_blocks[block_index].world_pixel_x - view_state.view_bottom_left_world_x + view_state.view_area_offset_x;
            int world_diff_y = game_state.world_blocks[block_index].world_pixel_y - view_state.view_bottom_left_world_y;
            int top_left_test_y = view_state.view_area_offset_y + view_state.view_height - world_diff_y - BLOCK_HEIGHT_IN_PIXELS;
            // printf("Using block pixel x and y, top-left on screen should be %d,%d\n", top_left_test_x, top_left_test_y);
            // TODO just for testing
            int sprite_index = game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * block_y_index + block_x_index].sprite_index;
            printf("write block %d,%d [sprite_index=%d] at pixel top-left %d,%d\n", block_x_index, block_y_index, sprite_index, top_left_x, top_left_y);
            fflush(stdout);
            write_sprite_aliased(   top_left_x,
                                    top_left_y,
                                    game_state.base_sprites[game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * block_y_index + block_x_index].sprite_index],
                                    false,
                                    width,
                                    r_pixels);
        }
    }

    // Draw top-left block
    if (left_block_x < 0 || top_block_y < 0 || left_block_x >= WIDTH_OF_WORLD_IN_BLOCKS || top_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // noop
        // printf("ttt Skip drawing top-left left_block_x=%d top_block_y=%d\n", left_block_x, top_block_y);
    } else {
        // printf("ttt do drawing top-left left_block_x=%d top_block_y=%d\n", left_block_x, top_block_y);
        write_sprite_aliased_subsection(    view_state.view_area_offset_x + left_block_num_x_cols_visible - BLOCK_WIDTH_IN_PIXELS,
                                            view_state.view_area_offset_y - BLOCK_HEIGHT_IN_PIXELS + top_block_num_y_rows_visible,
                                            game_state.base_sprites[game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * top_block_y + left_block_x].sprite_index],
                                            false, // flip_left_to_right
                                            BLOCK_WIDTH_IN_PIXELS - left_block_num_x_cols_visible,
                                            BLOCK_HEIGHT_IN_PIXELS - top_block_num_y_rows_visible,
                                            BLOCK_WIDTH_IN_PIXELS - 1,
                                            BLOCK_HEIGHT_IN_PIXELS - 1,
                                            width,
                                            r_pixels);
    }

    // Draw top-right block
    if (right_block_x < 0 || top_block_y < 0 || right_block_x >= WIDTH_OF_WORLD_IN_BLOCKS || top_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // noop
    } else {
    write_sprite_aliased_subsection(    view_state.view_area_offset_x + right_block_x_left_pixel_x - view_state.view_bottom_left_world_x,
                                        view_state.view_area_offset_y - BLOCK_HEIGHT_IN_PIXELS + top_block_num_y_rows_visible,
                                        game_state.base_sprites[game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * top_block_y + right_block_x].sprite_index],
                                        false, // flip_left_to_right
                                        0,
                                        BLOCK_HEIGHT_IN_PIXELS - top_block_num_y_rows_visible,
                                        right_block_num_x_cols_visible,
                                        BLOCK_HEIGHT_IN_PIXELS - 1,
                                        width,
                                        r_pixels);
    }

    // TODO just for testing
    struct Block bottom_left_block = game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * bottom_block_y + left_block_x];
    // printf("For bottom-left block, view_state.view_bottom_left_world_x=%d view_state.view_bottom_left_world_y=%d bottom_left_block.world_pixel_x=%d bottom_left_block.world_pixel_y=%d\n", view_state.view_bottom_left_world_x, view_state.view_bottom_left_world_y, bottom_left_block.world_pixel_x, bottom_left_block.world_pixel_y);
    // Draw bottom-left block
    if (left_block_x < 0 || bottom_block_y < 0 || left_block_x >= WIDTH_OF_WORLD_IN_BLOCKS || bottom_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // noop
    } else {
        write_sprite_aliased_subsection(    view_state.view_area_offset_x + left_block_num_x_cols_visible - BLOCK_WIDTH_IN_PIXELS,
                                            view_state.view_area_offset_y + view_state.view_height - bottom_block_num_y_rows_visible,
                                            game_state.base_sprites[game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * bottom_block_y + left_block_x].sprite_index],
                                            false, // flip_left_to_right
                                            BLOCK_WIDTH_IN_PIXELS - left_block_num_x_cols_visible,
                                            0,
                                            BLOCK_WIDTH_IN_PIXELS - 1,
                                            bottom_block_num_y_rows_visible,
                                            width,
                                            r_pixels);
    }

    // Draw bottom-right block
    if (right_block_x < 0 || bottom_block_y < 0 || right_block_x >= WIDTH_OF_WORLD_IN_BLOCKS || bottom_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // noop
    } else {
        write_sprite_aliased_subsection(    view_state.view_area_offset_x + view_state.view_width - right_block_num_x_cols_visible,
                                            view_state.view_area_offset_y + view_state.view_height - bottom_block_num_y_rows_visible,
                                            game_state.base_sprites[game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * bottom_block_y + right_block_x].sprite_index],
                                            false, // flip_left_to_right
                                            0,
                                            0,
                                            right_block_num_x_cols_visible,
                                            bottom_block_num_y_rows_visible,
                                            width,
                                            r_pixels);
    }

    // printf("bottom_block_num_y_rows_visible=%d bottom_block_y=%d\n", bottom_block_num_y_rows_visible, bottom_block_y);
    fflush(stdout);
    // Draw left col of blocks
    if (left_block_x < 0 || left_block_x >= WIDTH_OF_WORLD_IN_BLOCKS) {
        // Noop
        // printf("ttt Skip drawing left-col left_block_x=%d top_block_y=%d\n", left_block_x, top_block_y);
    } else {
        // printf("ttt do drawing left-col left_block_x=%d top_block_y=%d\n", left_block_x, top_block_y);
        for (int block_y_index = 1; block_y_index < top_block_y - bottom_block_y; ++block_y_index) {
            // printf("Consider drawing left col box at block_y=%d\n", block_y_index + bottom_block_y);
            if (block_y_index + bottom_block_y < 0 || block_y_index + bottom_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
                continue;
            }
            // printf("Actually draw left col box at block_y=%d\n", block_y_index + bottom_block_y);
            write_sprite_aliased_subsection(    view_state.view_area_offset_x - BLOCK_WIDTH_IN_PIXELS + left_block_num_x_cols_visible,
                                                view_state.view_area_offset_y + view_state.view_height - bottom_block_num_y_rows_visible - (block_y_index * BLOCK_HEIGHT_IN_PIXELS),
                                                game_state.base_sprites[game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * (block_y_index + bottom_block_y) + left_block_x].sprite_index],
                                                false, // flip_left_to_right
                                                BLOCK_WIDTH_IN_PIXELS - left_block_num_x_cols_visible,
                                                0,
                                                BLOCK_WIDTH_IN_PIXELS - 1,
                                                BLOCK_HEIGHT_IN_PIXELS - 1,
                                                width,
                                                r_pixels);
        }
    }

    // Draw right col of blocks
    if (right_block_x < 0 || right_block_x >= WIDTH_OF_WORLD_IN_BLOCKS) {
        // Noop
    } else {
        for (int block_y_index = bottom_block_y + 1; block_y_index < top_block_y; ++block_y_index) {
            // printf("Consider drawing right col box at block_y=%d\n", block_y_index);
            if (block_y_index < 0 || block_y_index >= HEIGHT_OF_WORLD_IN_BLOCKS) {
                continue;
            }
            // printf("Actually draw right col box at block_y=%d\n", block_y_index);
            int world_block_index = WIDTH_OF_WORLD_IN_BLOCKS * block_y_index + right_block_x;
            write_sprite_aliased_subsection(    view_state.view_area_offset_x + right_block_x_left_pixel_x - view_state.view_bottom_left_world_x,
                                                view_state.view_area_offset_y + view_state.view_height - (game_state.world_blocks[world_block_index].world_pixel_y - view_state.view_bottom_left_world_y) - BLOCK_HEIGHT_IN_PIXELS,
                                                game_state.base_sprites[game_state.world_blocks[world_block_index].sprite_index],
                                                false, // flip_left_to_right
                                                0,
                                                0,
                                                right_block_num_x_cols_visible,
                                                BLOCK_HEIGHT_IN_PIXELS - 1,
                                                width,
                                                r_pixels);
        }
    }

    // Draw top row of blocks
    if (top_block_y < 0 || top_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // Noop
    } else {
        for (int block_x_index = left_block_x + 1; block_x_index < right_block_x; ++block_x_index) {
            // printf("ttt draw top-row block for x=%d left_block_num_x_cols_visible=%d\n", block_x_index, left_block_num_x_cols_visible);
            if (block_x_index < 0 || block_x_index >= WIDTH_OF_WORLD_IN_BLOCKS) {
                // printf("ttt skip draw top-row block for x=%d\n", block_x_index);
                continue;
            }
            write_sprite_aliased_subsection(   view_state.view_area_offset_x + left_block_num_x_cols_visible + ((block_x_index - left_block_x - 1) * BLOCK_WIDTH_IN_PIXELS),
                                    view_state.view_area_offset_y - BLOCK_HEIGHT_IN_PIXELS + top_block_num_y_rows_visible,
                                    game_state.base_sprites[game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * top_block_y + block_x_index].sprite_index],
                                    false, // flip_left_to_right
                                    0,
                                    BLOCK_HEIGHT_IN_PIXELS - top_block_num_y_rows_visible,
                                    BLOCK_WIDTH_IN_PIXELS - 1,
                                    BLOCK_HEIGHT_IN_PIXELS - 1,
                                    width,
                                    r_pixels);
        }
    }

    // Draw bottom row of blocks
    if (bottom_block_y < 0 || bottom_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // Noop
    } else {
        for (int block_x_index = 1; block_x_index < right_block_x - left_block_x; ++block_x_index) {
            if (block_x_index + left_block_x < 0 || block_x_index + left_block_x >= WIDTH_OF_WORLD_IN_BLOCKS) {
                continue;
            }
            // printf("Get world block for bottom_block_y=%d left_block_x=%d block_x_index=%d\n", bottom_block_y, left_block_x, block_x_index);
            write_sprite_aliased_subsection(   view_state.view_area_offset_x + left_block_num_x_cols_visible + ((block_x_index - 1) * BLOCK_WIDTH_IN_PIXELS),
                                    view_state.view_area_offset_y + view_state.view_height - bottom_block_num_y_rows_visible,
                                    game_state.base_sprites[game_state.world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * bottom_block_y + left_block_x + block_x_index].sprite_index],
                                    false, // flip_left_to_right
                                    0,
                                    0,
                                    BLOCK_WIDTH_IN_PIXELS - 1,
                                    bottom_block_num_y_rows_visible,
                                    width,
                                    r_pixels);
        }
    }

    int char_left = round(game_state.character.x_bottom_left) - view_state.view_bottom_left_world_x + view_state.view_area_offset_x;
    int char_top =      view_state.view_bottom_left_world_y
                    +   view_state.view_height
                    -   game_state.character.y_inverted_bottom_left
                    +   view_state.view_area_offset_y
                    -   game_state.character.height;
    // printf("Draw character at %d,%d\n", char_left, char_top);
    write_sprite_aliased(char_left, char_top, game_state.character.current_sprite, game_state.character.direction == LEFT, width, r_pixels);
}

// IMPLEMENTS
void process_frame_and_blit(long frame_count, long current_time_in_micros, uint32_t *pixels, int width, int height) {
    game_state.current_frame = frame_count;
    game_state.current_time_in_micros = current_time_in_micros;

    game_state.character.is_on_ground = is_on_ground(&game_state);

    handle_input(&game_state, &view_state, &input_state, &editor_state, game_state.world_rules.microseconds_per_frame);
    // printf("character.x_velocity_pixels_per_second is now %f\n", game_state.character.x_velocity_pixels_per_second);

    do_movement(&game_state, game_state.world_rules.microseconds_per_frame);

    update_sprites(&game_state, &input_state);

    // printf("character.x is now %f\n", game_state.character.x_bottom_left);

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

}

// IMPLEMENTS
bool should_stop() {
    return !still_running;
}

// IMPLEMENTS
void cleanup() {
    free(blank_pixels);
}