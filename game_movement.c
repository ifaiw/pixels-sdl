#include "game_movement.h"

#include <inttypes.h>

#include <SDL2/SDL.h>

#include "game_state.h"


// PRIVATE
inline void get_new_location(struct Character* current, double* r_new_x, double* r_new_y) {
    //current.
}

// IMPLEMENTS
void do_movement(struct Character* character, double microseconds_to_advance) {
    printf("do_movement frame_time_in_micros as double: %f\n", microseconds_to_advance);
    character->center_x += character->x_velocity_pixels_per_second / (double)1000000 * microseconds_to_advance;
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
        switch (game_state->character.motion) {
            case STOPPED_WALK_RIGHT:
            case STOPPED_WALK_LEFT:
                game_state->character.motion = START_WALK_RIGHT;
                // game_state->character.x_velocity_pixels_per_second = world_rules->x_movement_initial_speed_pixels_per_second;
                game_state->character.x_velocity_pixels_per_second += world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case START_WALK_RIGHT:
                game_state->character.motion = WALK_RIGHT;
                // game_state->character.x_velocity_pixels_per_second = world_rules->x_movement_next_speed_1_pixels_per_second;
                game_state->character.x_velocity_pixels_per_second += world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case WALK_RIGHT:
                game_state->character.motion = WALK_RIGHT;
                // game_state->character.x_velocity_pixels_per_second = world_rules->x_movement_next_speed_1_pixels_per_second;
                game_state->character.x_velocity_pixels_per_second += world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case STOPPING_WALK_RIGHT:
                game_state->character.motion = WALK_RIGHT;
                // game_state->character.x_velocity_pixels_per_second = world_rules->x_movement_next_speed_1_pixels_per_second;
                game_state->character.x_velocity_pixels_per_second += world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case START_WALK_LEFT:
                game_state->character.motion = STOPPED_WALK_RIGHT;
                // game_state->character.x_velocity_pixels_per_second = 0;
                game_state->character.x_velocity_pixels_per_second += world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case WALK_LEFT:
                game_state->character.motion = STOPPING_WALK_LEFT;
                // game_state->character.x_velocity_pixels_per_second = 0;
                game_state->character.x_velocity_pixels_per_second += world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case STOPPING_WALK_LEFT:
                game_state->character.motion = STOPPED_WALK_RIGHT;
                // game_state->character.x_velocity_pixels_per_second = 0;
                game_state->character.x_velocity_pixels_per_second += world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
        }
        printf("right key pressed down frame %ld, now motion is %d\n", game_state->current_frame, game_state->character.motion);
    } else {
        if (input_state->right_button_press_frame != -1) {
            printf("right button released\n");
        }
        input_state->right_button_press_frame = -1;

        switch (game_state->character.motion) {
            case STOPPED_WALK_RIGHT:
                game_state->character.motion = STOPPED_WALK_RIGHT;
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
            case START_WALK_RIGHT:
                game_state->character.motion = STOPPED_WALK_RIGHT;
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
            case STOPPING_WALK_RIGHT:
                game_state->character.motion = STOPPED_WALK_RIGHT;
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
            case WALK_RIGHT:
                game_state->character.motion = STOPPING_WALK_RIGHT;
                // game_state->character.x_velocity_pixels_per_second = world_rules->x_movement_initial_speed_pixels_per_second;
                game_state->character.x_velocity_pixels_per_second -= world_rules->x_ground_deacceleration_pixels_per_second * seconds_to_advance;
                if (game_state->character.x_velocity_pixels_per_second < 0) {
                    game_state->character.x_velocity_pixels_per_second = 0;
                }
                break;
            // If we're currently doing left motion and the right key stops being pressed, ignore it
            case STOPPED_WALK_LEFT:
            case STOPPING_WALK_LEFT:
            case WALK_LEFT:
            case START_WALK_LEFT:
                break;
        }
    }

    if (state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT]){
        printf("left arrow key is pressed\n");
        if (input_state->left_button_press_frame == -1) {
            input_state->left_button_press_frame = game_state->current_frame;
        }
        switch (game_state->character.motion) {
            case STOPPED_WALK_LEFT:
            case STOPPED_WALK_RIGHT:
                game_state->character.motion = START_WALK_LEFT;
                // game_state->character.x_velocity_pixels_per_second = -world_rules->x_movement_initial_speed_pixels_per_second;
                game_state->character.x_velocity_pixels_per_second -= world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case START_WALK_LEFT:
                game_state->character.motion = WALK_LEFT;
                // game_state->character.x_velocity_pixels_per_second = -world_rules->x_movement_next_speed_1_pixels_per_second;
                game_state->character.x_velocity_pixels_per_second -= world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case WALK_LEFT:
                game_state->character.motion = WALK_LEFT;
                // game_state->character.x_velocity_pixels_per_second = -world_rules->x_movement_next_speed_1_pixels_per_second;
                game_state->character.x_velocity_pixels_per_second -= world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case STOPPING_WALK_LEFT:
                game_state->character.motion = WALK_LEFT;
                // game_state->character.x_velocity_pixels_per_second = -world_rules->x_movement_next_speed_1_pixels_per_second;
                game_state->character.x_velocity_pixels_per_second -= world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case START_WALK_RIGHT:
                game_state->character.motion = STOPPED_WALK_LEFT;
                // game_state->character.x_velocity_pixels_per_second = 0;
                game_state->character.x_velocity_pixels_per_second -= world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case WALK_RIGHT:
                game_state->character.motion = STOPPING_WALK_RIGHT;
                // game_state->character.x_velocity_pixels_per_second = 0;
                game_state->character.x_velocity_pixels_per_second -= world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                break;
            case STOPPING_WALK_RIGHT:
                game_state->character.motion = STOPPED_WALK_LEFT;
                // game_state->character.x_velocity_pixels_per_second = 0;
                game_state->character.x_velocity_pixels_per_second -= world_rules->x_ground_deacceleration_pixels_per_second * seconds_to_advance;
                break;
        }
        printf("left key pressed down frame %ld, now motion is %d\n", game_state->current_frame, game_state->character.motion);
    } else {
        if (input_state->left_button_press_frame != -1) {
            printf("left button released\n");
        }
        input_state->left_button_press_frame = -1;

        switch (game_state->character.motion) {
            case STOPPED_WALK_LEFT:
                game_state->character.motion = STOPPED_WALK_LEFT;
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
            case START_WALK_LEFT:
                game_state->character.motion = STOPPED_WALK_LEFT;
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
            case STOPPING_WALK_LEFT:
                game_state->character.motion = STOPPED_WALK_LEFT;
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
            case WALK_LEFT:
                game_state->character.motion = STOPPING_WALK_LEFT;
                // game_state->character.x_velocity_pixels_per_second = -world_rules->x_movement_initial_speed_pixels_per_second;
                game_state->character.x_velocity_pixels_per_second += world_rules->x_ground_acceleration_pixels_per_second * seconds_to_advance;
                if (game_state->character.x_velocity_pixels_per_second > 0) {
                    game_state->character.x_velocity_pixels_per_second = 0;
                }
                break;
            // If we're currently doing right motion and the left key stops being pressed, ignore it
            case STOPPED_WALK_RIGHT:
            case STOPPING_WALK_RIGHT:
            case WALK_RIGHT:
            case START_WALK_RIGHT:
                break;
        }
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