#include "game_movement.h"

#include <inttypes.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "game_editor.h"
#include "game_paths.h"


// SDL_SCANCODE_A = 4
uint32_t LETTER_SCANCODE_MASKS[36] = {
    0,0,0,0,  // First letter scancode, A, is 4, so pad first 4 elements of this array
    0b00000000000000000000000000000001,  // SDL_SCANCODE_A = 4
    0b00000000000000000000000000000010,
    0b00000000000000000000000000000100,
    0b00000000000000000000000000001000,
    0b00000000000000000000000000010000,
    0b00000000000000000000000000100000,
    0b00000000000000000000000001000000,
    0b00000000000000000000000010000000,
    0b00000000000000000000000100000000,
    0b00000000000000000000001000000000,
    0b00000000000000000000010000000000,
    0b00000000000000000000100000000000,
    0b00000000000000000001000000000000,
    0b00000000000000000010000000000000,
    0b00000000000000000100000000000000,
    0b00000000000000001000000000000000,
    0b00000000000000010000000000000000,
    0b00000000000000100000000000000000,
    0b00000000000001000000000000000000,
    0b00000000000010000000000000000000,
    0b00000000000100000000000000000000,
    0b00000000001000000000000000000000,
    0b00000000010000000000000000000000,
    0b00000000100000000000000000000000,
    0b00000001000000000000000000000000,
    0b00000010000000000000000000000000,  // SDL_SCANCODE_Z = 29
    0b00000100000000000000000000000000,  // From here down is unused, but leaving here so it can easily be copy/pasted for other bitmask arrays
    0b00001000000000000000000000000000,
    0b00010000000000000000000000000000,
    0b00100000000000000000000000000000,
    0b01000000000000000000000000000000,
    0b10000000000000000000000000000000
};
uint32_t LETTER_SCANCODE_MASKS_NEGATED[36] = {
    0,0,0,0,  // First letter scancode, A, is 4, so pad first 4 elements of this array
    0b11111111111111111111111111111110,  // SDL_SCANCODE_A = 4
    0b11111111111111111111111111111101,
    0b11111111111111111111111111111011,
    0b11111111111111111111111111110111,
    0b11111111111111111111111111101111,
    0b11111111111111111111111111011111,
    0b11111111111111111111111110111111,
    0b11111111111111111111111101111111,
    0b11111111111111111111111011111111,
    0b11111111111111111111110111111111,
    0b11111111111111111111101111111111,
    0b11111111111111111111011111111111,
    0b11111111111111111110111111111111,
    0b11111111111111111101111111111111,
    0b11111111111111111011111111111111,
    0b11111111111111110111111111111111,
    0b11111111111111101111111111111111,
    0b11111111111111011111111111111111,
    0b11111111111110111111111111111111,
    0b11111111111101111111111111111111,
    0b11111111111011111111111111111111,
    0b11111111110111111111111111111111,
    0b11111111101111111111111111111111,
    0b11111111011111111111111111111111,
    0b11111110111111111111111111111111,
    0b11111101111111111111111111111111,  // SDL_SCANCODE_Z = 29
    0b11111011111111111111111111111111,  // From here down is unused, but leaving here so it can easily be copy/pasted for other bitmask arrays
    0b11110111111111111111111111111111,
    0b11101111111111111111111111111111,
    0b11011111111111111111111111111111,
    0b10111111111111111111111111111111,
    0b01111111111111111111111111111111
};

uint16_t NUMBER_SCANCODE_MASKS[16] = {
    0b0000000000000001,  // SDL_SCANCODE_1 = 30, need to subtract SDL_SCANCODE_1 when indexing into these masks
    0b0000000000000010,
    0b0000000000000100,
    0b0000000000001000,
    0b0000000000010000,
    0b0000000000100000,
    0b0000000001000000,
    0b0000000010000000,
    0b0000000100000000,
    0b0000001000000000,  // SDL_SCANCODE_0 = 39, codes under here are currently unused
    0b0000010000000000,
    0b0000100000000000,
    0b0001000000000000,
    0b0010000000000000,
    0b0100000000000000,
    0b1000000000000000,
};
uint16_t NUMBER_SCANCODE_MASKS_NEGATED[16] = {
    0b1111111111111110,  // SDL_SCANCODE_1 = 30, need to subtract SDL_SCANCODE_1 when indexing into these masks
    0b1111111111111101,
    0b1111111111111011,
    0b1111111111110111,
    0b1111111111101111,
    0b1111111111011111,
    0b1111111110111111,
    0b1111111101111111,
    0b1111111011111111,
    0b1111110111111111,  // SDL_SCANCODE_0 = 39, codes under here are currently unused
    0b1111101111111111,
    0b1111011111111111,
    0b1110111111111111,
    0b1101111111111111,
    0b1011111111111111,
    0b0111111111111111,
};

// PRIVATE
// Find the world block that the player is entirely
struct Block* get_vertical_middle_block(struct GameState* game_state) {


    return NULL;
}

// IMPLEMENTS
void do_movement(struct GameState* game_state, double microseconds_to_advance) {
    // printf("do_movement x_velocity is %f y_velocity is %f frame_time_in_micros as double: %f\n", game_state->character.x_velocity_pixels_per_second, game_state->character.y_velocity_pixels_per_second, microseconds_to_advance);
    double new_character_x = game_state->character.x_bottom_left + game_state->character.x_velocity_pixels_per_second / (double)1000000 * microseconds_to_advance;
    double new_character_y = game_state->character.y_inverted_bottom_left + game_state->character.y_velocity_pixels_per_second / (double)1000000 * microseconds_to_advance;

    // printf("movement: old_x %f old_y %f new_x %f new_y %f\n", game_state->character.x_bottom_left, game_state->character.y_inverted_bottom_left, new_character_x, new_character_y);
    // TODO just for testing
    if (new_character_y < game_state->character.y_inverted_bottom_left) {
        // printf("At top of do_movement moving-down\n");
    }

    int old_character_x_floor = floor(game_state->character.x_bottom_left);
    int old_character_y_floor = floor(game_state->character.y_inverted_bottom_left);
    int right_pixel_old = old_character_x_floor + game_state->character.width;
    int left_pixel_old = old_character_x_floor;
    int bottom_pixel_old = old_character_y_floor;
    int top_pixel_old = old_character_y_floor + game_state->character.height;

    int new_character_x_floor = floor(new_character_x);
    int new_character_y_floor = floor(new_character_y);
    int right_pixel_new = new_character_x_floor + game_state->character.width;
    int left_pixel_new = new_character_x_floor;
    int bottom_pixel_new = new_character_y_floor;
    int top_pixel_new = new_character_y_floor + game_state->character.height;

    bool x_motion_stopped = false;
    bool y_motion_stopped_above = false;
    bool y_motion_stopped_below = false;

    double x_collision_distance = -1;
    double y_collision_distance = -1;

    // MOVING RIGHT COLLISION CHECK
    if (new_character_x > game_state->character.x_bottom_left) {
        // Need to check block at feet height and block at head height
        struct Block* new_block_feet = get_world_block_for_location(right_pixel_new, bottom_pixel_old, game_state);
        struct Block* new_block_head = get_world_block_for_location(right_pixel_new, top_pixel_old, game_state);
        // Need to handle the case where player is falling and new_character_y is currently below the top of the solid block below,
        // but the block above that block is not solid. We don't want to use the solid block below to check for left/right collision
        if (new_block_feet->effects_flags & EFFECT_FLAG_SOLID) {
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_feet);
            // TODO not needed? x_collision_distance = block_bottom_left.x - new_character_x + game_state->character.width;

            new_character_x = block_bottom_left.x - game_state->character.width - 0.000001;
            x_motion_stopped = true;
        }
        if (new_block_head->effects_flags & EFFECT_FLAG_SOLID) {
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_head);
            // TODO not needed?
            // double temp_x_collision_distance = block_bottom_left.x - new_character_x + game_state->character.width;
            // if (x_collision_distance >= 0 && temp_x_collision_distance < x_collision_distance) {
            //     x_collision_distance = block_bottom_left.x - new_character_x + game_state->character.width;
            // }

            new_character_x = block_bottom_left.x - game_state->character.width - 0.000001;
            x_motion_stopped = true;
        }
    }
    // MOVING LEFT COLLISION CHECK
    else if (new_character_x < game_state->character.x_bottom_left) {
        // printf("Moving left, get blocks for pixels %d,%d and %d,%d\n", left_pixel_new, bottom_pixel_old, left_pixel_new, top_pixel_old);
        // Need to check block at feet height and block at head height
        struct Block* new_block_feet = get_world_block_for_location(left_pixel_new, bottom_pixel_old, game_state);
        struct Block* new_block_head = get_world_block_for_location(left_pixel_new, top_pixel_old, game_state);
        // printf("Feet and head blocks are %d,%d and %d,%d\n", new_block_feet->world_x, new_block_feet->world_y, new_block_head->world_x, new_block_head->world_y);
        if (new_block_feet->effects_flags & EFFECT_FLAG_SOLID) {
            // printf("feet block is solid\n");
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_feet);
            // printf("bottom-left of block is %d,%d\n", block_bottom_left.x, block_bottom_left.y);
            new_character_x = block_bottom_left.x + BLOCK_WIDTH_IN_PIXELS;
            x_motion_stopped = true;
        }
        else if (new_block_head->effects_flags & EFFECT_FLAG_SOLID) {
            // printf("head block is solid\n");
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_head);
            // printf("bottom-left of block is %d,%d\n", block_bottom_left.x, block_bottom_left.y);
            new_character_x = block_bottom_left.x + BLOCK_WIDTH_IN_PIXELS;
            x_motion_stopped = true;
        }
    }

    // MOVING DOWN COLLISION CHECK
    if (new_character_y < game_state->character.y_inverted_bottom_left) {
        // printf("do_movement collision check moving-down\n");
        struct Block* new_block_left = get_world_block_for_location(left_pixel_old, bottom_pixel_new, game_state);
        struct Block* new_block_right = get_world_block_for_location(right_pixel_old, bottom_pixel_new, game_state);
        if (new_block_left->effects_flags & EFFECT_FLAG_SOLID) {
            // printf("do_movement collision check moving-down TRUE left block is solid\n");
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_left);
            // TODO is this extra chek needed here and for the other block? Don't we already know at this point that the character is past the edge of the block?
            if (new_character_y < block_bottom_left.y + BLOCK_HEIGHT_IN_PIXELS) {
                new_character_y = block_bottom_left.y + BLOCK_HEIGHT_IN_PIXELS;
                // printf("do_movement collision check moving-down left block is solid new_character_y to %f\n", new_character_y);
            }
            y_motion_stopped_below = true;
        }
        if (new_block_right->effects_flags & EFFECT_FLAG_SOLID) {
            // printf("do_movement collision check moving-down TRUE right block is solid\n");
            struct XY block_bottom_right = get_bottom_left_world_pixel_for_block(new_block_right);
            if (new_character_y < block_bottom_right.y + BLOCK_HEIGHT_IN_PIXELS) {
                new_character_y = block_bottom_right.y + BLOCK_HEIGHT_IN_PIXELS;
                // printf("do_movement collision check moving-down right block is solid new_character_y to %f\n", new_character_y);
            }
            y_motion_stopped_below = true;
        }
    }

    // MOVING UP COLLISION CHECK
    else if (new_character_y > game_state->character.y_inverted_bottom_left) {
        // printf("do_movement collision check moving-up\n");
        struct Block* new_block_left = get_world_block_for_location(left_pixel_old, top_pixel_new, game_state);
        struct Block* new_block_right = get_world_block_for_location(right_pixel_old, top_pixel_new, game_state);
        if (new_block_left->effects_flags & EFFECT_FLAG_SOLID) {
            // printf("do_movement collision check moving-up TRUE left block is solid\n");
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_left);
            new_character_y = block_bottom_left.y - game_state->character.height - 1;
            // printf("do_movement collision check moving-up left block is solid new_character_y to %f\n", new_character_y);
            y_motion_stopped_above = true;
        }
        if (new_block_right->effects_flags & EFFECT_FLAG_SOLID) {
            // printf("do_movement collision check moving-up TRUE right block is solid\n");
            struct XY block_bottom_right = get_bottom_left_world_pixel_for_block(new_block_right);
            new_character_y = block_bottom_right.y - game_state->character.height - 1;
            // printf("do_movement collision check moving-up right block is solid new_character_y to %f\n", new_character_y);
            y_motion_stopped_above = true;
        }
    }

    // Moved off climbing check
    if (game_state->character.motion == CLIMBING && !is_on_climable(game_state)) {
        game_state->character.motion = JUMPING;
    }

    // TODO Shouln't switch to stopped if just one axis is blocked but still moving in other axis
    if (y_motion_stopped_above) {
        printf("do_movement y_motion_stopped_above is true, current motion is %d\n", game_state->character.motion);
        game_state->character.y_velocity_pixels_per_second = 0;
        // TODO how can motion get stopped above if character is not jumping?
        if (game_state->character.motion != JUMPING) {
            printf("do_movement y_motion_stopped_above is true motion_change to STOPPED\n");
            game_state->character.motion = STOPPED;
        }
    }

    if (x_motion_stopped) {
        printf("do_movement x_motion_stopped is true, current motion is %d\n", game_state->character.motion);
        game_state->character.x_velocity_pixels_per_second = 0;
        if (game_state->character.motion != JUMPING) {
            printf("do_movement x_motion_stopped is true motion_change to STOPPED\n");
            game_state->character.motion = STOPPED;
        }
    }

    if (y_motion_stopped_below) {
        printf("do_movement y_motion_stopped_below is true, current motion is %d, x velocity is %f\n", game_state->character.motion, game_state->character.x_velocity_pixels_per_second);
        game_state->character.y_velocity_pixels_per_second = 0;

        // TODO Should this just check x_motion_stopped?
        if (game_state->character.x_velocity_pixels_per_second != 0) {
            printf("do_movement y_motion_stopped_below is true but x velocity is not 0, motion_change to WALKING\n");
            game_state->character.motion = WALKING;
        } else {
            printf("do_movement y_motion_stopped_below is true and x velocity is 0, motion_change to STOPPED\n");
            game_state->character.motion = STOPPED;
        }
    }

    // printf("In do_movement set x_bottom_left=%f y_inverted_bottom_left=%f\n", new_character_x, new_character_y);

    game_state->character.x_bottom_left = new_character_x;
    game_state->character.y_inverted_bottom_left = new_character_y;

    // printf("Check if is on ground before applying gravity\n");
    if (!is_on_ground(game_state) && game_state->character.motion != CLIMBING) {
        printf("not on ground or climbing, apply gravity\n");
        game_state->character.y_velocity_pixels_per_second -= game_state->world_rules.gravity_pixels_per_second;
        if (game_state->character.y_velocity_pixels_per_second < -game_state->world_rules.y_max_fall_speed_pixels_per_second) {
            game_state->character.y_velocity_pixels_per_second = -game_state->world_rules.y_max_fall_speed_pixels_per_second;
        }
        printf("now y_velocity_pixels_per_second is %f\n", game_state->character.y_velocity_pixels_per_second);
    }
    // TODO just for testing
    else {
        // printf("on ground or climbing, DO NOT apply gravity\n");
    }

    // Climbing stops x and y motion after movement applied
    if (game_state->character.motion == CLIMBING) {
        game_state->character.x_velocity_pixels_per_second = 0;
        game_state->character.y_velocity_pixels_per_second = 0;
    }
}

// IMPLEMENTS
void handle_input(  struct GameState* game_state,
                    struct ViewState* view_state,
                    struct InputState* input_state,
                    struct EditorState* editor_state,
                    double microseconds_to_advance)
{
    double seconds_to_advance = microseconds_to_advance / (double)1000000;
// Handle specific key presses from scancodes
    const Uint8* state = SDL_GetKeyboardState(NULL);

    bool any_key_pressed = false;
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (state[i]) {
            any_key_pressed = true;
            break;
        }
    }
    printf("any_key_pressed %d\n", any_key_pressed);

    struct EditorState previous_editor_state = *editor_state;

    if(state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_LEFT]){
        // printf("right arrow key is pressed\n");

        if (input_state->right_button_press_frame == -1) {
            input_state->right_button_press_frame = game_state->current_frame;
        }

        // TODO Handle in air case, and possibly consolidate?
        switch (game_state->character.direction) {
            case RIGHT:
                switch (game_state->character.motion) {
                    case STOPPED:
                        printf("handle_input spot 1 motion_change to WALKING\n");
                        game_state->character.motion = WALKING;
                        game_state->character.x_velocity_pixels_per_second += game_state->world_rules.x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case WALKING:
                        game_state->character.x_velocity_pixels_per_second += game_state->world_rules.x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case JUMPING:
                        game_state->character.x_velocity_pixels_per_second += game_state->world_rules.x_air_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case CLIMBING:
                        game_state->character.x_velocity_pixels_per_second = game_state->world_rules.x_climb_speed_pixels_per_second;
                        break;
                }
                break;
            case LEFT:
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
        }
        game_state->character.direction = RIGHT;
        // printf("right key pressed down frame %ld, now motion is %d\n", game_state->current_frame, game_state->character.motion);
    } else { // TODO this doesn't seem right, need better check for right button released?
        if (input_state->right_button_press_frame != -1) {
            printf("right button released\n");
        }
        input_state->right_button_press_frame = -1;

        switch (game_state->character.direction) {
            case RIGHT:
                if (game_state->character.motion == WALKING) {
                    printf("right button released I think, motion from WALKING to STOPPED\n");
                    printf("handle_input spot 2 motion_change to STOPPED\n");
                    game_state->character.motion = STOPPED;
                    game_state->character.x_velocity_pixels_per_second = 0;
                }
                break;
            case LEFT:
                break;
        }
    }

    if (state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT]){
        // printf("left arrow key is pressed\n");
        if (input_state->left_button_press_frame == -1) {
            input_state->left_button_press_frame = game_state->current_frame;
        }
        // TODO Handle in air case, and possibly consolidate?
        switch (game_state->character.direction) {
            case LEFT:
                switch (game_state->character.motion) {
                    case STOPPED:
                        printf("handle_input spot 3 motion_change to WALKING\n");
                        game_state->character.motion = WALKING;
                        game_state->character.x_velocity_pixels_per_second -= game_state->world_rules.x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case WALKING:
                        game_state->character.x_velocity_pixels_per_second -= game_state->world_rules.x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case JUMPING:
                        game_state->character.x_velocity_pixels_per_second -= game_state->world_rules.x_air_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case CLIMBING:
                        game_state->character.x_velocity_pixels_per_second = -game_state->world_rules.x_climb_speed_pixels_per_second;
                        break;
                }
                break;
            case RIGHT:
                game_state->character.x_velocity_pixels_per_second = 0;
                break;
        }
        game_state->character.direction = LEFT;
        // printf("left key pressed down frame %ld, now motion is %d\n", game_state->current_frame, game_state->character.motion);
    } else { // TODO this doesn't seem right, need better check for left button released?
        if (input_state->left_button_press_frame != -1) {
            printf("left button released\n");
        }
        input_state->left_button_press_frame = -1;

        switch (game_state->character.direction) {
            case LEFT:
                if (game_state->character.motion == WALKING) {
                    printf("left button released I think, motion from WALKING to STOPPED\n");
                    printf("handle_input spot 4 motion_change to STOPPED\n");
                    game_state->character.motion = STOPPED;
                    game_state->character.x_velocity_pixels_per_second = 0;
                }
                break;
            case RIGHT:
                break;
        }
    }

    if (state[SDL_SCANCODE_UP]) {
        switch (game_state->character.motion)
        {
            case STOPPED:
            case WALKING:
            case JUMPING:
                if (is_on_climable(game_state)) {
                    game_state->character.motion = CLIMBING;
                    input_state->start_climb_pixel_x = game_state->character.x_bottom_left;
                    input_state->start_climb_pixel_y = game_state->character.y_inverted_bottom_left;
                    // Setting x and y velocities to 0 should happen in do_movement now that motion is CLIMBING
                }
                break;
            case CLIMBING:
                game_state->character.y_velocity_pixels_per_second = game_state->world_rules.y_climb_speed_pixels_per_second;
                break;
            default:
                break;
            }
    }

    if(state[SDL_SCANCODE_SPACE]) {
        // printf("space key is pressed\n");

        bool motion_allows_jump = false;
        switch(game_state->character.motion) {
            case STOPPED:
            case WALKING:
                motion_allows_jump = true;
                break;
            default:
                // printf("Jump not allowed because motion is %d\n", game_state->character.motion);
                break;
        }

        printf("up arrow pressed, y_velocity_pixels_per_second=%f is_jumping? %d motion=%d is_on_ground=%d\n", game_state->character.y_velocity_pixels_per_second, game_state->character.motion == JUMPING, game_state->character.motion, game_state->character.is_on_ground);

        if (game_state->character.y_velocity_pixels_per_second == 0 && game_state->character.is_on_ground && motion_allows_jump) {
            // printf("Start jump\n");
            // Start jump
            game_state->character.y_velocity_pixels_per_second = game_state->world_rules.y_jump_acceleration_pixels_per_second;
            game_state->character.micros_when_jump_started = game_state->current_time_in_micros;
            game_state->character.motion = JUMPING;
            printf("start jump, motion_change to JUMPING y_velocity_pixels_per_second is now %f\n", game_state->world_rules.y_jump_acceleration_pixels_per_second);
        }
        // Holding down the up arrow a little longer makes the character jump higher
        else if (game_state->character.y_velocity_pixels_per_second > 0 && game_state->character.motion == JUMPING) {
            printf("up button still pressed, check if we can jump more\n");
            // TODO tidy?
            long micros_since_jump_start = game_state->current_time_in_micros - game_state->character.micros_when_jump_started;
            printf("micros_since_jump_start is %ld\n", micros_since_jump_start);
            if (micros_since_jump_start < game_state->world_rules.microseconds_after_jump_start_check_jump_still_pressed) {
                game_state->character.y_velocity_pixels_per_second = game_state->world_rules.y_jump_acceleration_pixels_per_second;
                printf("Jumping higher, y_velocity_pixels_per_second to %f\n", game_state->character.y_velocity_pixels_per_second);
            }
        }
    }
    else if (state[SDL_SCANCODE_DOWN]){
        switch (game_state->character.motion)
        {
            case WALKING:
            case JUMPING:
                if (is_on_climable(game_state)) {
                    game_state->character.motion = CLIMBING;
                    // Setting x and y velocities to 0 should happen in do_movement now that motion is CLIMBING
                }
                break;
            case CLIMBING:
                game_state->character.y_velocity_pixels_per_second = -game_state->world_rules.y_climb_speed_pixels_per_second;
                break;
            default:
                break;
            }
    }

    if (state[SDL_SCANCODE_L]) {
        if (!(input_state->letter_keys_down_bitmask & LETTER_SCANCODE_MASKS[SDL_SCANCODE_L])) {
            input_state->letter_keys_down_bitmask |= LETTER_SCANCODE_MASKS[SDL_SCANCODE_L];
        }
    } else {
        if (input_state->letter_keys_down_bitmask & LETTER_SCANCODE_MASKS[SDL_SCANCODE_L]) {
            input_state->letter_keys_down_bitmask &= LETTER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_L];
            editor_state->load_on_num = true;
        } else {
            if (any_key_pressed) {
                editor_state->load_on_num = false;
            }
        }
    }

    if (state[SDL_SCANCODE_R]) {
        if (!(input_state->letter_keys_down_bitmask & LETTER_SCANCODE_MASKS[SDL_SCANCODE_R])) {
            input_state->letter_keys_down_bitmask |= LETTER_SCANCODE_MASKS[SDL_SCANCODE_R];
            load_world_rules_from_file(&game_state->world_rules);
        }
    } else {
        if (input_state->letter_keys_down_bitmask & LETTER_SCANCODE_MASKS[SDL_SCANCODE_R]) {
            input_state->letter_keys_down_bitmask &= LETTER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_R];
        }
    }

    if (state[SDL_SCANCODE_S]) {
        printf("scancode s\n");
        if (!(input_state->letter_keys_down_bitmask & LETTER_SCANCODE_MASKS[SDL_SCANCODE_S])) {
            printf("s pressed down\n");
            input_state->letter_keys_down_bitmask |= LETTER_SCANCODE_MASKS[SDL_SCANCODE_S];
        }
    } else {
        printf("no scancode s\n");
        if (input_state->letter_keys_down_bitmask & LETTER_SCANCODE_MASKS[SDL_SCANCODE_S]) {
            printf("s released, save_on_num to true\n");
            input_state->letter_keys_down_bitmask &= LETTER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_S];
            editor_state->save_on_num = true;
        } else {
            if (any_key_pressed) {
                printf("some key is pressed, set save_on_num to false\n");
                editor_state->save_on_num = false;
            }
        }
    }

    if (state[SDL_SCANCODE_1]) {
        if (!(input_state->number_keys_down_bitmask & NUMBER_SCANCODE_MASKS[SDL_SCANCODE_1 - SDL_SCANCODE_1])) {
            printf("1 key pressed down. prev load_on_num %d prev save_on_num %d\n", previous_editor_state.load_on_num, previous_editor_state.save_on_num);
            input_state->number_keys_down_bitmask |= NUMBER_SCANCODE_MASKS[SDL_SCANCODE_1 - SDL_SCANCODE_1];
            if (previous_editor_state.load_on_num) {
                printf("load level slot 1\n");
                load_level(game_state, 1);
            }
            else if (previous_editor_state.save_on_num) {
                printf("save level slot 1\n");
                save_level(game_state, 1);
            }
            else {
                editor_state->block_type = BLOCK_TYPE_GROUND;
            }
        }
    } else {
        input_state->number_keys_down_bitmask &= NUMBER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_1 - SDL_SCANCODE_1];
    }

    if (state[SDL_SCANCODE_2]) {
        if (!(input_state->number_keys_down_bitmask & NUMBER_SCANCODE_MASKS[SDL_SCANCODE_2 - SDL_SCANCODE_1])) {
            // printf("2 key pressed down\n");
            input_state->number_keys_down_bitmask |= NUMBER_SCANCODE_MASKS[SDL_SCANCODE_2 - SDL_SCANCODE_1];
            if (previous_editor_state.load_on_num) {
                printf("load level slot 2\n");
                load_level(game_state, 2);
            }
            else if (previous_editor_state.save_on_num) {
                printf("save level slot 2\n");
                save_level(game_state, 2);
            }
            else {
                editor_state->block_type = BLOCK_TYPE_LADDER;
            }
        }
    } else {
        input_state->number_keys_down_bitmask &= NUMBER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_2 - SDL_SCANCODE_1];
    }

    if (state[SDL_SCANCODE_3]) {
        if (!(input_state->number_keys_down_bitmask & NUMBER_SCANCODE_MASKS[SDL_SCANCODE_3 - SDL_SCANCODE_1])) {
            // printf("3 key pressed down");
            input_state->number_keys_down_bitmask |= NUMBER_SCANCODE_MASKS[SDL_SCANCODE_3 - SDL_SCANCODE_1];
            if (previous_editor_state.load_on_num) {
                printf("load level slot 3");
                load_level(game_state, 3);
            }
            else if (previous_editor_state.save_on_num) {
                printf("save level slot 3");
                save_level(game_state, 3);
            }
            else {
                // Switch current editor block to something
            }
        }
    } else {
        input_state->number_keys_down_bitmask &= NUMBER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_3 - SDL_SCANCODE_1];
    }

    if (state[SDL_SCANCODE_4]) {
        if (!(input_state->number_keys_down_bitmask & NUMBER_SCANCODE_MASKS[SDL_SCANCODE_4 - SDL_SCANCODE_1])) {
            // printf("4 key pressed down");
            input_state->number_keys_down_bitmask |= NUMBER_SCANCODE_MASKS[SDL_SCANCODE_4 - SDL_SCANCODE_1];
            if (previous_editor_state.load_on_num) {
                printf("load level slot 4");
                load_level(game_state, 4);
            }
            else if (previous_editor_state.save_on_num) {
                printf("save level slot 4");
                save_level(game_state, 4);
            }
            else {
                // Switch current editor block to something
            }
        }
    } else {
        input_state->number_keys_down_bitmask &= NUMBER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_4 - SDL_SCANCODE_1];
    }

    if (state[SDL_SCANCODE_5]) {
        if (!(input_state->number_keys_down_bitmask & NUMBER_SCANCODE_MASKS[SDL_SCANCODE_5 - SDL_SCANCODE_1])) {
            // printf("5 key pressed down");
            input_state->number_keys_down_bitmask |= NUMBER_SCANCODE_MASKS[SDL_SCANCODE_5 - SDL_SCANCODE_1];
            if (previous_editor_state.load_on_num) {
                printf("load level slot 5");
                load_level(game_state, 5);
            }
            else if (previous_editor_state.save_on_num) {
                printf("save level slot 5");
                save_level(game_state, 5);
            }
            else {
                // Switch current editor block to something
            }
        }
    } else {
        input_state->number_keys_down_bitmask &= NUMBER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_5 - SDL_SCANCODE_1];
    }

    if (state[SDL_SCANCODE_6]) {
        if (!(input_state->number_keys_down_bitmask & NUMBER_SCANCODE_MASKS[SDL_SCANCODE_6 - SDL_SCANCODE_1])) {
            // printf("6 key pressed down");
            input_state->number_keys_down_bitmask |= NUMBER_SCANCODE_MASKS[SDL_SCANCODE_6 - SDL_SCANCODE_1];
            if (previous_editor_state.load_on_num) {
                printf("load level slot 6");
                load_level(game_state, 6);
            }
            else if (previous_editor_state.save_on_num) {
                printf("save level slot 6");
                save_level(game_state, 6);
            }
            else {
                // Switch current editor block to something
            }
        }
    } else {
        input_state->number_keys_down_bitmask &= NUMBER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_6 - SDL_SCANCODE_1];
    }

    if (state[SDL_SCANCODE_7]) {
        if (!(input_state->number_keys_down_bitmask & NUMBER_SCANCODE_MASKS[SDL_SCANCODE_7 - SDL_SCANCODE_1])) {
            // printf("7 key pressed down");
            input_state->number_keys_down_bitmask |= NUMBER_SCANCODE_MASKS[SDL_SCANCODE_7 - SDL_SCANCODE_1];
            if (previous_editor_state.load_on_num) {
                printf("load level slot 7");
                load_level(game_state, 7);
            }
            else if (previous_editor_state.save_on_num) {
                printf("save level slot 7");
                save_level(game_state, 7);
            }
            else {
                // Switch current editor block to something
            }
        }
    } else {
        input_state->number_keys_down_bitmask &= NUMBER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_7 - SDL_SCANCODE_1];
    }

    if (state[SDL_SCANCODE_8]) {
        if (!(input_state->number_keys_down_bitmask & NUMBER_SCANCODE_MASKS[SDL_SCANCODE_8 - SDL_SCANCODE_1])) {
            // printf("8 key pressed down");
            input_state->number_keys_down_bitmask |= NUMBER_SCANCODE_MASKS[SDL_SCANCODE_8 - SDL_SCANCODE_1];
            if (previous_editor_state.load_on_num) {
                printf("load level slot 8");
                load_level(game_state, 8);
            }
            else if (previous_editor_state.save_on_num) {
                printf("save level slot 8");
                save_level(game_state, 8);
            }
            else {
                // Switch current editor block to something
            }
        }
    } else {
        input_state->number_keys_down_bitmask &= NUMBER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_8 - SDL_SCANCODE_1];
    }

    if (state[SDL_SCANCODE_9]) {
        if (!(input_state->number_keys_down_bitmask & NUMBER_SCANCODE_MASKS[SDL_SCANCODE_9 - SDL_SCANCODE_1])) {
            // printf("9 key pressed down");
            input_state->number_keys_down_bitmask |= NUMBER_SCANCODE_MASKS[SDL_SCANCODE_9 - SDL_SCANCODE_1];
            if (previous_editor_state.load_on_num) {
                printf("load level slot 9");
                load_level(game_state, 9);
            }
            else if (previous_editor_state.save_on_num) {
                printf("save level slot 9");
                save_level(game_state, 9);
            }
            else {
                // Switch current editor block to something
            }
        }
    } else {
        input_state->number_keys_down_bitmask &= NUMBER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_9 - SDL_SCANCODE_1];
    }

    if (state[SDL_SCANCODE_0]) {
        if (!(input_state->number_keys_down_bitmask & NUMBER_SCANCODE_MASKS[SDL_SCANCODE_0 - SDL_SCANCODE_1])) {
            // printf("0 key pressed down");
            input_state->number_keys_down_bitmask |= NUMBER_SCANCODE_MASKS[SDL_SCANCODE_0 - SDL_SCANCODE_1];
            if (previous_editor_state.load_on_num) {
                printf("load level slot 0");
                load_level(game_state, 0);
            }
            else if (previous_editor_state.save_on_num) {
                printf("save level slot 0");
                save_level(game_state, 0);
            }
            else {
                // Switch current editor block to something
            }
        }
    } else {
        input_state->number_keys_down_bitmask &= NUMBER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_0 - SDL_SCANCODE_1];
    }

    int new_mouse_x;
    int new_mouse_y;
    uint32_t new_mouse_button_state = SDL_GetMouseState(&new_mouse_x, &new_mouse_y);
    // printf("SDL_GetMouseState button=%d x=%d y=%d\n", new_mouse_button_state, new_mouse_x, new_mouse_y);
    input_state->mouse_x = new_mouse_x;
    input_state->mouse_y = new_mouse_y;
    if (new_mouse_button_state != input_state->mouse_button_state) {
        // printf("Mouse button change from %d to %d\n", input_state->mouse_button_state, new_mouse_button_state);
        if (new_mouse_button_state == 1 && input_state->mouse_button_state == 0) {
            // printf("mouse left-button click at %d,%d\n", new_mouse_x, new_mouse_y);
            int in_game_x = new_mouse_x - view_state->view_area_offset_x + view_state->view_bottom_left_world_x;
            int in_game_y_inverted = view_state->view_height - new_mouse_y + view_state->view_area_offset_y + view_state->view_bottom_left_world_y;
            // printf("mouse left-button click at %d,%d translated to ingame is %d,%d\n", new_mouse_x, new_mouse_y, in_game_x, in_game_y_inverted);
            struct Block* mouse_block = get_world_block_for_location(in_game_x, in_game_y_inverted, game_state);
            if (mouse_block != NULL) {
                // printf("clicked block effects_flag is %d\n", mouse_block->effects_flags);
                if (mouse_block->effects_flags & EFFECT_FLAG_SOLID || mouse_block->type == BLOCK_TYPE_LADDER) {
                    printf("remove ground or ladder at %d,%d\n", mouse_block->block_x, mouse_block->block_y);
                    mouse_block->type = BLOCK_TYPE_EMPTY;
                    mouse_block->effects_flags = game_state->base_blocks[BLOCK_TYPE_EMPTY].effects_flags;
                    mouse_block->sprite_index = game_state->base_blocks[BLOCK_TYPE_EMPTY].sprite_index;
                } else if (mouse_block->type == BLOCK_TYPE_EMPTY) {
                    if (editor_state->block_type == BLOCK_TYPE_GROUND) {
                        printf("add ground at %d,%d\n", mouse_block->block_x, mouse_block->block_y);
                        mouse_block->type = BLOCK_TYPE_GROUND;
                        mouse_block->effects_flags = game_state->base_blocks[BLOCK_TYPE_GROUND].effects_flags;
                        mouse_block->sprite_index = game_state->base_blocks[BLOCK_TYPE_GROUND].sprite_index;
                    }
                    else if (editor_state->block_type == BLOCK_TYPE_LADDER) {
                        printf("add ladder at %d,%d\n", mouse_block->block_x, mouse_block->block_y);
                        mouse_block->type = BLOCK_TYPE_LADDER;
                        mouse_block->effects_flags = game_state->base_blocks[BLOCK_TYPE_LADDER].effects_flags;
                        mouse_block->sprite_index = game_state->base_blocks[BLOCK_TYPE_LADDER].sprite_index;
                    }
                    else if (editor_state->block_type == BLOCK_TYPE_TOILET) {
                        printf("add toilet at %d,%d\n", mouse_block->block_x, mouse_block->block_y);
                        mouse_block->type = BLOCK_TYPE_TOILET;
                        mouse_block->effects_flags = game_state->base_blocks[BLOCK_TYPE_TOILET].effects_flags;
                        mouse_block->sprite_index = game_state->base_blocks[BLOCK_TYPE_TOILET].sprite_index;
                    }
                }
                update_ground_images(game_state);

                // autosave level
                save_level(game_state, 10);
            } else {
                // printf("No block at click\n");
            }
        }
        input_state->mouse_button_state = new_mouse_button_state;
    }

    // printf("Bottom of handle_input game_state->character.x_velocity_pixels_per_second=%f\n", game_state->character.x_velocity_pixels_per_second);
    if (game_state->character.x_velocity_pixels_per_second > game_state->world_rules.x_movement_max_speed_pixels_per_second) {
        // printf("At max x speed right\n");
        game_state->character.x_velocity_pixels_per_second = game_state->world_rules.x_movement_max_speed_pixels_per_second;
    }
    else if (game_state->character.x_velocity_pixels_per_second < -game_state->world_rules.x_movement_max_speed_pixels_per_second) {
        // printf("At max x speed left\n");
        game_state->character.x_velocity_pixels_per_second = -game_state->world_rules.x_movement_max_speed_pixels_per_second;
    }
}