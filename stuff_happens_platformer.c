#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "stuff_happens.h"

#include "game_blocks.h"
#include "game_editor.h"
#include "game_images.h"
#include "game_movement.h"
#include "game_paths.h"
#include "game_renderer.h"
#include "game_sprites.h"
#include "game_state.h"
#include "graphics_constants.h"
#include "file_stuff.h"
#include "utils.h"

#define INITIAL_FRAMES_TO_WAIT 180

static long micros_per_frame;

// block grid 25 x 18 is 1200 x 864

static bool still_running;

// TODO just for testing
int last_walking_frame;

struct GameState game_state;
struct ViewState view_state;
struct InputState input_state;
struct EditorState editor_state;

struct CharacterSprite character_sprite;


// PRIVATE
void initialize_world_rules(double frames_per_second, struct WorldRules* world_rules) {
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
void initialize_input_state() {
    input_state.left_button_press_frame = -1;
    input_state.right_button_press_frame = -1;
    input_state.up_button_press_frame = -1;
    input_state.down_button_press_frame = -1;

    input_state.letter_keys_down_bitmask = 0;
    input_state.number_keys_down_bitmask = 0;
    input_state.mouse_button_state = 0;
}

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

    // Level slot 10 is autosave
    load_level(&game_state, 10);
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
    game_state.character_sprite.num_climbing_animation_frames = 12;
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

    initialize_input_state();

    // 60 frames per second
    // fps / 1000000 = frames per micros
    // 1000000/fps = micros per frame

    game_state.blank_pixels = (uint32_t*)malloc(height * width * 4);
    for (int i = 0; i < height * width; ++i) {
        game_state.blank_pixels[i] = ALPHA;
    }

    editor_state.block_type = BLOCK_TYPE_LADDER;

    // printf("bottom of initialize\n");
    return 0;
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
    blit(pixels, &game_state, &view_state, width, height);
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
}

// IMPLEMENTS
bool should_stop() {
    return !still_running;
}

// IMPLEMENTS
void cleanup() {
    free(game_state.blank_pixels);
}