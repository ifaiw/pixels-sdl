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
};

struct Sprite {
    uint32_t* pixels_start;
    uint32_t height, width;
    uint32_t image_source_pitch_in_pixels;
    bool flip_left_to_right;
};

struct Block {
    int world_x;
    int world_y;
    uint16_t type;
    uint32_t effects_flags;
    struct Sprite sprite;
};

enum CharacterMotion {
    WALKING,
    STOPPED
};

enum CharacterDirection {
    LEFT,
    RIGHT,
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
    double gravity_pixels_per_frame;
    double x_movement_initial_speed_pixels_per_second;
    double x_movement_next_speed_1_pixels_per_second;
    // TODO max x speed not needed?
    double x_movement_max_speed_pixels_per_second;
    double x_movement_initial_speed_pixels_per_frame;
    double x_movement_next_speed_1_pixels_per_frame;
    // TODO max x speed not needed?
    double x_movement_max_speed_pixels_per_frame;

    double x_ground_acceleration_pixels_per_second;
    double x_ground_deacceleration_pixels_per_second;

    double y_jump_acceleration_pixels_per_second;

    int num_walking_animation_frames;
    long micros_per_walking_animation_frame;
};

struct XY{
    int x;
    int y;
};

#endif  // _GAME_STRUCTS__H