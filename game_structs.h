#ifndef _GAME_STRUCTS__H
#define _GAME_STRUCTS__H

#include <stdbool.h>
#include <inttypes.h>

struct ImageInfo {
    int width;
    int height;
    uint32_t* pixels;
};

struct InputState {
    long left_button_press_frame;
    long right_button_press_frame;
    long up_button_press_frame;
    long down_button_press_frame;
    uint32_t letter_keys_down_bitmask;
    uint16_t number_keys_down_bitmask;
    uint32_t mouse_button_state;
    int mouse_x;
    int mouse_y;
};

struct EditorState {
    uint16_t block_type;
};

struct Sprite {
    uint32_t* pixels_start;
    uint32_t height, width;
    uint32_t image_source_pitch_in_pixels;
    bool flip_left_to_right;
    // TODO only used for testing?
    int sprite_index;
};

struct Block {
    int world_pixel_x; // Bottom-left
    int world_pixel_y; // Bottom-left
    int block_x;
    int block_y;
    uint16_t type;
    uint32_t effects_flags;
    struct Sprite sprite;
};

enum CharacterMotion {
    WALKING,
    STOPPED,
    JUMPING,
    CLIMBING,
};

enum CharacterDirection {
    LEFT,
    RIGHT,
};

struct CharacterSprite {
    int stand_sprite_index;
    int first_walk_sprite_index;
    int num_walking_animation_frames;
};

struct Character {
    double x_bottom_left;
    double y_inverted_bottom_left;
    double x_velocity_pixels_per_second;
    double y_velocity_pixels_per_second;
    double width;
    double height;
    struct Sprite current_sprite;
    enum CharacterMotion motion;
    enum CharacterDirection direction;
    long micros_when_jump_started;
    bool is_on_ground;
};

struct WorldRules {
    double frames_per_second;
    double microseconds_per_frame;
    double gravity_pixels_per_second;
    double y_max_fall_speed_pixels_per_second;
    // TODO max x speed not needed?
    double x_movement_max_speed_pixels_per_second;
    // TODO max x speed not needed?
    double x_movement_max_speed_pixels_per_frame;

    double x_ground_acceleration_pixels_per_second;
    double x_ground_deacceleration_pixels_per_second;

    double x_air_acceleration_pixels_per_second;
    double x_air_deacceleration_pixels_per_second;

    double y_jump_acceleration_pixels_per_second;

    double y_climb_speed_pixels_per_second;

    double microseconds_after_jump_start_check_jump_still_pressed;

    long micros_per_walking_animation_frame;
};

struct XY{
    int x;
    int y;
};

struct ViewState {
    int window_width;
    int window_height;
    int view_width;
    int view_height;
    // Pixel offset of the top-left corner of the area where the world is rendered in the window
    // y is not inverted, it is the number of pixels down from the top of the window
    int view_area_offset_x;
    int view_area_offset_y;

    int view_bottom_left_world_x;
    int view_bottom_left_world_y;
};

#endif  // _GAME_STRUCTS__H