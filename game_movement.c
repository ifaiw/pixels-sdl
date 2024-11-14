#include "game_movement.h"

#include <inttypes.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "game_state.h"


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


// IMPLEMENTS
void do_movement(struct GameState* game_state, double microseconds_to_advance) {
    printf("do_movement x_velocity is %f y_velocity is %f frame_time_in_micros as double: %f\n", game_state->character.x_velocity_pixels_per_second, game_state->character.y_velocity_pixels_per_second, microseconds_to_advance);
    double new_character_x = game_state->character.x_bottom_left + game_state->character.x_velocity_pixels_per_second / (double)1000000 * microseconds_to_advance;
    double new_character_y = game_state->character.y_inverted_bottom_left + game_state->character.y_velocity_pixels_per_second / (double)1000000 * microseconds_to_advance;

    printf("movement: old_x %f old_y %f new_x %f new_y %f\n", game_state->character.x_bottom_left, game_state->character.y_inverted_bottom_left, new_character_x, new_character_y);
    // TODO just for testing
    if (new_character_y < game_state->character.y_inverted_bottom_left) {
        printf("At top of do_movement moving-down\n");
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
    bool y_motion_stopped = false;

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

            new_character_x = block_bottom_left.x - game_state->character.width - 1;
            x_motion_stopped = true;
        }
        if (new_block_head->effects_flags & EFFECT_FLAG_SOLID) {
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_head);
            // TODO not needed?
            // double temp_x_collision_distance = block_bottom_left.x - new_character_x + game_state->character.width;
            // if (x_collision_distance >= 0 && temp_x_collision_distance < x_collision_distance) {
            //     x_collision_distance = block_bottom_left.x - new_character_x + game_state->character.width;
            // }

            new_character_x = block_bottom_left.x - game_state->character.width - 1;
            x_motion_stopped = true;
        }
    }
    // MOVING LEFT COLLISION CHECK
    else if (new_character_x < game_state->character.x_bottom_left) {
        printf("Moving left, get blocks for pixels %d,%d and %d,%d\n", left_pixel_new, bottom_pixel_old, left_pixel_new, top_pixel_old);
        // Need to check block at feet height and block at head height
        struct Block* new_block_feet = get_world_block_for_location(left_pixel_new, bottom_pixel_old, game_state);
        struct Block* new_block_head = get_world_block_for_location(left_pixel_new, top_pixel_old, game_state);
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

    // MOVING DOWN COLLISION CHECK
    if (new_character_y < game_state->character.y_inverted_bottom_left) {
        printf("do_movement collision check moving-down\n");
        struct Block* new_block_left = get_world_block_for_location(left_pixel_old, bottom_pixel_new, game_state);
        struct Block* new_block_right = get_world_block_for_location(right_pixel_old, bottom_pixel_new, game_state);
        if (new_block_left->effects_flags & EFFECT_FLAG_SOLID) {
            printf("do_movement collision check moving-down TRUE left block is solid\n");
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_left);
            // TODO is this extra chek needed here and for the other block? Don't we already know at this point that the character is past the edge of the block?
            if (new_character_y < block_bottom_left.y + BLOCK_HEIGHT_IN_PIXELS) {
                new_character_y = block_bottom_left.y + BLOCK_HEIGHT_IN_PIXELS;
                printf("do_movement collision check moving-down left block is solid new_character_y to %f\n", new_character_y);
            }
            y_motion_stopped = true;
        }
        if (new_block_right->effects_flags & EFFECT_FLAG_SOLID) {
            printf("do_movement collision check moving-down TRUE right block is solid\n");
            struct XY block_bottom_right = get_bottom_left_world_pixel_for_block(new_block_right);
            if (new_character_y < block_bottom_right.y + BLOCK_HEIGHT_IN_PIXELS) {
                new_character_y = block_bottom_right.y + BLOCK_HEIGHT_IN_PIXELS;
                printf("do_movement collision check moving-down right block is solid new_character_y to %f\n", new_character_y);
            }
            y_motion_stopped = true;
        }
    }

    // MOVING UP COLLISION CHECK
    else if (new_character_y > game_state->character.y_inverted_bottom_left) {
        printf("do_movement collision check moving-up\n");
        struct Block* new_block_left = get_world_block_for_location(left_pixel_old, top_pixel_new, game_state);
        struct Block* new_block_right = get_world_block_for_location(right_pixel_old, top_pixel_new, game_state);
        if (new_block_left->effects_flags & EFFECT_FLAG_SOLID) {
            printf("do_movement collision check moving-up TRUE left block is solid\n");
            struct XY block_bottom_left = get_bottom_left_world_pixel_for_block(new_block_left);
            new_character_y = block_bottom_left.y - game_state->character.height - 1;
            printf("do_movement collision check moving-up left block is solid new_character_y to %f\n", new_character_y);
            y_motion_stopped = true;
        }
        if (new_block_right->effects_flags & EFFECT_FLAG_SOLID) {
            printf("do_movement collision check moving-up TRUE right block is solid\n");
            struct XY block_bottom_right = get_bottom_left_world_pixel_for_block(new_block_right);
            new_character_y = block_bottom_right.y - game_state->character.height - 1;
            printf("do_movement collision check moving-up right block is solid new_character_y to %f\n", new_character_y);
            y_motion_stopped = true;
        }
    }


    // TODO Shouln't switch to stopped if just one axis is blocked but still moving in other axis
    if (y_motion_stopped) {
        printf("do_movement y_motion_stopped is true\n");
        game_state->character.y_velocity_pixels_per_second = 0;
        game_state->character.motion = STOPPED;
    }

    if (x_motion_stopped) {
        game_state->character.x_velocity_pixels_per_second = 0;
        game_state->character.motion = STOPPED;
    }

    printf("In do_movement set x_bottom_left=%f y_inverted_bottom_left=%f\n", new_character_x, new_character_y);

    game_state->character.x_bottom_left = new_character_x;
    game_state->character.y_inverted_bottom_left = new_character_y;

    printf("Check if is on ground before applying gravity\n");
    if (!is_on_ground(game_state)) {
        printf("not on ground, apply gravity\n");
        game_state->character.y_velocity_pixels_per_second -= game_state->world_rules.gravity_pixels_per_second;
        if (game_state->character.y_velocity_pixels_per_second < -game_state->world_rules.y_max_fall_speed_pixels_per_second) {
            game_state->character.y_velocity_pixels_per_second = -game_state->world_rules.y_max_fall_speed_pixels_per_second;
        }
    }
    // TODO just for testing
    else {
        printf("on ground, DO NOT apply gravity\n");
    }
}

// IMPLEMENTS
void handle_input(struct GameState* game_state, struct InputState* input_state, double microseconds_to_advance) {
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
                        game_state->character.x_velocity_pixels_per_second += game_state->world_rules.x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case WALKING:
                        game_state->character.x_velocity_pixels_per_second += game_state->world_rules.x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case JUMPING:
                        game_state->character.x_velocity_pixels_per_second += game_state->world_rules.x_air_acceleration_pixels_per_second * seconds_to_advance;
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
                        game_state->character.x_velocity_pixels_per_second -= game_state->world_rules.x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case WALKING:
                        game_state->character.x_velocity_pixels_per_second -= game_state->world_rules.x_ground_acceleration_pixels_per_second * seconds_to_advance;
                        break;
                    case JUMPING:
                        game_state->character.x_velocity_pixels_per_second -= game_state->world_rules.x_air_acceleration_pixels_per_second * seconds_to_advance;
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

        bool motion_allows_jump = false;
        switch(game_state->character.motion) {
            case STOPPED:
            case WALKING:
                motion_allows_jump = true;
                break;
            default:
                printf("Jump not allowed because motion is %d\n", game_state->character.motion);
                break;
        }

        printf("up arrow pressed, y_velocity_pixels_per_second=%f is_on_ground=%d\n", game_state->character.y_velocity_pixels_per_second, game_state->character.is_on_ground);

        if (game_state->character.y_velocity_pixels_per_second == 0 && game_state->character.is_on_ground && motion_allows_jump) {
            printf("Start jump\n");
            // Start jump
            game_state->character.y_velocity_pixels_per_second = game_state->world_rules.y_jump_acceleration_pixels_per_second;
            game_state->character.micros_when_jump_started = game_state->current_time_in_micros;
            game_state->character.motion = JUMPING;
        }
        // Holding down the up arrow a little longer makes the character jump higher
        else if (game_state->character.y_velocity_pixels_per_second > 0 && game_state->character.motion == JUMPING) {
            printf("up button still pressed, check if we can jump more\n");
            // TODO tidy?
            long micros_since_jump_start = game_state->current_time_in_micros - game_state->character.micros_when_jump_started;
            printf("micros_since_jump_start is %ld\n", micros_since_jump_start);
            if (micros_since_jump_start < game_state->world_rules.microseconds_after_jump_start_check_jump_still_pressed) {
                printf("Jumping higher\n");
                game_state->character.y_velocity_pixels_per_second = game_state->world_rules.y_jump_acceleration_pixels_per_second;
            }
        }
    }
    else if (state[SDL_SCANCODE_DOWN]){
        printf("down arrow key is pressed, ignored for now\n");
        // gravity_pixels_per_second -= 1;
        // gravity_pixels_per_frame = (double)gravity_pixels_per_second / (double)1000000 * (double)micros_per_frame;
    }

    if (state[SDL_SCANCODE_L]) {
        if (!(input_state->letter_keys_down_bitmask & LETTER_SCANCODE_MASKS[SDL_SCANCODE_L])) {
            printf("L key pressed down\n");
            input_state->letter_keys_down_bitmask |= LETTER_SCANCODE_MASKS[SDL_SCANCODE_L];
            load_world_rules_from_file(&game_state->world_rules);
        }
    } else {
        input_state->letter_keys_down_bitmask &= LETTER_SCANCODE_MASKS_NEGATED[SDL_SCANCODE_L];
    }

    int new_mouse_x;
    int new_mouse_y;
    uint32_t new_mouse_button_state = SDL_GetMouseState(&new_mouse_x, &new_mouse_y);
    printf("SDL_GetMouseState button=%d x=%d y=%d\n", new_mouse_button_state, new_mouse_x, new_mouse_y);
    input_state->mouse_x = new_mouse_x;
    input_state->mouse_y = new_mouse_y;
    if (new_mouse_button_state != input_state->mouse_button_state) {
        printf("Mouse button change from %d to %d\n", input_state->mouse_button_state, new_mouse_button_state);
        if (new_mouse_button_state == 1 && input_state->mouse_button_state == 0) {
            printf("mouse left-button click at %d,%d\n", new_mouse_x, new_mouse_y);
            int in_game_x = new_mouse_x - game_state->blocks_area_offset_x;
            int in_game_y_inverted = WORLD_BLOCKS_HEIGHT * BLOCK_HEIGHT_IN_PIXELS - (new_mouse_y - game_state->blocks_area_offset_y);
            printf("mouse left-button click at %d,%d translated to ingame is %d,%d\n", new_mouse_x, new_mouse_y, in_game_x, in_game_y_inverted);
            struct Block* mouse_block = get_world_block_for_location(in_game_x, in_game_y_inverted, game_state);
            if (mouse_block != NULL) {
                printf("clicked block effects_flag is %d\n", mouse_block->effects_flags);
                if (mouse_block->effects_flags & EFFECT_FLAG_SOLID) {
                    mouse_block->type = BLOCK_TYPE_EMPTY;
                    mouse_block->effects_flags = game_state->base_blocks[BLOCK_TYPE_EMPTY].effects_flags;
                    mouse_block->sprite = game_state->base_blocks[BLOCK_TYPE_EMPTY].sprite;
                } else {
                    mouse_block->type = BLOCK_TYPE_GROUND;
                    mouse_block->effects_flags = game_state->base_blocks[BLOCK_TYPE_GROUND].effects_flags;
                    mouse_block->sprite = game_state->base_blocks[BLOCK_TYPE_GROUND].sprite;
                }
            } else {
                printf("No block at click\n");
            }
        }
        input_state->mouse_button_state = new_mouse_button_state;
    }

    printf("Bottom of handle_input game_state->character.x_velocity_pixels_per_second=%f\n", game_state->character.x_velocity_pixels_per_second);
    if (game_state->character.x_velocity_pixels_per_second > game_state->world_rules.x_movement_max_speed_pixels_per_second) {
        printf("At max x speed right\n");
        game_state->character.x_velocity_pixels_per_second = game_state->world_rules.x_movement_max_speed_pixels_per_second;
    }
    else if (game_state->character.x_velocity_pixels_per_second < -game_state->world_rules.x_movement_max_speed_pixels_per_second) {
        printf("At max x speed left\n");
        game_state->character.x_velocity_pixels_per_second = -game_state->world_rules.x_movement_max_speed_pixels_per_second;
    }
}