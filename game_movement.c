#include "game_movement.h"

#include <inttypes.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "game_state.h"


// PRIVATE
inline void get_new_location(struct Character* current, double* r_new_x, double* r_new_y) {
    //current.
}

// IMPLEMENTS
void do_movement(struct GameState* game_state, double microseconds_to_advance) {
    printf("do_movement x_velocity is %f frame_time_in_micros as double: %f\n", game_state->character.x_velocity_pixels_per_second, microseconds_to_advance);
    double new_character_x = game_state->character.x_bottom_left + game_state->character.x_velocity_pixels_per_second / (double)1000000 * microseconds_to_advance;
    double new_character_y = game_state->character.y_inverted_bottom_left + game_state->character.y_velocity_pixels_per_second / (double)1000000 * microseconds_to_advance;

    printf("movement: old x %f old y %f new x %f new y %f\n", game_state->character.x_bottom_left, game_state->character.y_inverted_bottom_left, new_character_x, new_character_y);

    int new_character_x_rounded = round(new_character_x);
    int new_character_y_rounded = round(new_character_y);
    int right_pixel = new_character_x_rounded + game_state->character.width;
    int left_pixel = new_character_x_rounded;
    int bottom_pixel = new_character_y_rounded;
    int top_pixel = new_character_y_rounded + game_state->character.height;

    bool x_motion_stopped = false;

    if (new_character_x > game_state->character.x_bottom_left) {
        // Need to check block at feet height and block at head height
        struct Block* new_block_feet = get_world_block_for_location(right_pixel, bottom_pixel, game_state);
        struct Block* new_block_head = get_world_block_for_location(right_pixel, top_pixel, game_state);
        if (new_block_feet->effects_flags & EFFECT_FLAG_SOLID) {
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_feet);
            new_character_x = block_bottom_left.x - game_state->character.width;
            x_motion_stopped = true;
        }
        else if (new_block_head->effects_flags & EFFECT_FLAG_SOLID) {
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_head);
            new_character_x = block_bottom_left.x - game_state->character.width;
            x_motion_stopped = true;
        }
    }
    else if (new_character_x < game_state->character.x_bottom_left) {
        printf("Moving left, get blocks for pixels %d,%d and %d,%d\n", left_pixel, bottom_pixel, left_pixel, top_pixel);
        // Need to check block at feet height and block at head height
        struct Block* new_block_feet = get_world_block_for_location(left_pixel, bottom_pixel, game_state);
        struct Block* new_block_head = get_world_block_for_location(left_pixel, top_pixel, game_state);
        printf("Feet and head blocks are %d,%d and %d,%d\n", new_block_feet->world_x, new_block_feet->world_y, new_block_head->world_x, new_block_head->world_y);
        if (new_block_feet->effects_flags & EFFECT_FLAG_SOLID) {
            printf("feet block is solid\n");
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_feet);
            printf("bottom-left of block is %d,%d\n", block_bottom_left.x, block_bottom_left.y);
            new_character_x = block_bottom_left.x + BLOCK_WIDTH_IN_PIXELS;
            x_motion_stopped = true;
        }
        else if (new_block_head->effects_flags & EFFECT_FLAG_SOLID) {
            printf("head block is solid\n");
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_head);
            printf("bottom-left of block is %d,%d\n", block_bottom_left.x, block_bottom_left.y);
            new_character_x = block_bottom_left.x + BLOCK_WIDTH_IN_PIXELS;
            x_motion_stopped = true;
        }
    }

    if (x_motion_stopped) {
        game_state->character.x_velocity_pixels_per_second = 0;
        game_state->character.motion = STOPPED;
    }

    game_state->character.x_bottom_left = new_character_x;
    game_state->character.y_inverted_bottom_left = new_character_y;
}

// IMPLEMENTS
void handle_input(struct GameState* game_state, struct InputState* input_state, struct WorldRules* world_rules, double microseconds_to_advance) {
    double seconds_to_advance = microseconds_to_advance / (double)1000000;
// Handle specific key presses from scancodes
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_LEFT]){
        printf("right arrow key is pressed\n");

        if (input_state->right_button_press_frame == -1) {
            input_state->right_button_press_frame = game_state->current_frame;
        }

        // TODO Handle in air case, and possibly consolidate?
        switch (game_state->character.direction) {
            case RIGHT:
                switch (game_state->character.motion) {
                    case STOPPED:
                        game_state->character.motion = WALKING;
                        // game_state->character.x_velocity_pixels_per_second = world_rules->x_movement_initial_speed_pixels_per_second;
                        game_state->character.x_velocity_pixels_per_second += world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case WALKING:
                        // game_state->character.x_velocity_pixels_per_second = world_rules->x_movement_next_speed_1_pixels_per_second;
                        game_state->character.x_velocity_pixels_per_second += world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                }
                break;
            case LEFT:
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
        }
        game_state->character.direction = RIGHT;
        printf("right key pressed down frame %ld, now motion is %d\n", game_state->current_frame, game_state->character.motion);
    } else {
        if (input_state->right_button_press_frame != -1) {
            printf("right button released\n");
        }
        input_state->right_button_press_frame = -1;

        switch (game_state->character.direction) {
            case RIGHT:
                game_state->character.motion = STOPPED;
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
            case LEFT:
                break;
        }
    }

    if (state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT]){
        printf("left arrow key is pressed\n");
        if (input_state->left_button_press_frame == -1) {
            input_state->left_button_press_frame = game_state->current_frame;
        }
        // TODO Handle in air case, and possibly consolidate?
        switch (game_state->character.direction) {
            case LEFT:
                switch (game_state->character.motion) {
                    case STOPPED:
                        game_state->character.motion = WALKING;
                        game_state->character.x_velocity_pixels_per_second -= world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case WALKING:
                        game_state->character.x_velocity_pixels_per_second -= world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                }
                break;
            case RIGHT:
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
        }
        game_state->character.direction = LEFT;
        printf("left key pressed down frame %ld, now motion is %d\n", game_state->current_frame, game_state->character.motion);
    } else {
        if (input_state->left_button_press_frame != -1) {
            printf("left button released\n");
        }
        input_state->left_button_press_frame = -1;

        switch (game_state->character.direction) {
            case LEFT:
                game_state->character.motion = STOPPED;
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
            case RIGHT:
                break;
        }
    }

    if(state[SDL_SCANCODE_UP]){
        printf("up arrow key is pressed\n");

        if (game_state->character.y_velocity_pixels_per_second == 0 && is_on_ground(game_state)) {
            // Start jump
            game_state->character.y_velocity_pixels_per_second = world_rules->y_jump_acceleration_pixels_per_second;
            micros_when_jump_started = game_state->current_time_in_micros;
        }
    }
    else if (state[SDL_SCANCODE_DOWN]){
        printf("down arrow key is pressed, ignored for now\n");
        // gravity_pixels_per_second -= 1;
        // gravity_pixels_per_frame = (double)gravity_pixels_per_second / (double)1000000 * (double)micros_per_frame;
    }

    printf("Bottom of handle_input game_state->character.x_velocity_pixels_per_second=%f\n", game_state->character.x_velocity_pixels_per_second);
    if (game_state->character.x_velocity_pixels_per_second > world_rules->x_movement_max_speed_pixels_per_second) {
        printf("At max x speed right\n");
        game_state->character.x_velocity_pixels_per_second = world_rules->x_movement_max_speed_pixels_per_second;
    }
    else if (game_state->character.x_velocity_pixels_per_second < -world_rules->x_movement_max_speed_pixels_per_second) {
        printf("At max x speed left\n");
        game_state->character.x_velocity_pixels_per_second = -world_rules->x_movement_max_speed_pixels_per_second;
    }
}