#include <math.h>
#include <stdbool.h>

#include "stuff_happens.h"

#include "graphics_constants.h"

#define X_SPEED_CHANGE_RATE 1
#define Y_SPEED_CHANGE_RATE 1

static int ball_x;
static int ball_y;
static int x_speed = 0;
static int y_speed = 0;

static bool still_running;

static uint32_t *blank_pixels;

static uint32_t dull_red;

// PRIVATE
inline void draw_circle(
    uint32_t *pixels,
    int window_width,
    int window_height,
    int circle_x,
    int circle_y,
    int radius,
    uint32_t colour)
{
    if (circle_x - radius < 0) {
        circle_x = radius;
    }
    else if (circle_x + radius >= window_width) {
        circle_x = window_width - radius - 1;
    }

    if (circle_y - radius < 0) {
        circle_y = radius;
    }
    else if (circle_y + radius >= window_height) {
        circle_y = window_height - radius - 1;
    }

    double r = radius;
    int x = 0;
    int y = round(sqrt((double)(r * r - (double)(x * x))));
    while (x <= y) {
        pixels[x + circle_x + ((y + circle_y) * window_width)] = colour;
        pixels[x + circle_x + ((-y + circle_y) * window_width)] = colour;
        pixels[-x + circle_x + ((y + circle_y) * window_width)] = colour;
        pixels[-x + circle_x + ((-y + circle_y) * window_width)] = colour;

        pixels[y + circle_x + ((x + circle_y) * window_width)] = colour;
        pixels[y + circle_x + ((-x + circle_y) * window_width)] = colour;
        pixels[-y + circle_x + ((x + circle_y) * window_width)] = colour;
        pixels[-y + circle_x + ((-x + circle_y) * window_width)] = colour;

        x += 1;
        y = round(sqrt((double)(r * r - (double)(x * x))));

        // if (debug_first_circle) {
        //     printf("x=%d y=%d", x, y);
        // }
    }
}

// PRIVATE
inline void blit(uint32_t *pixels, int width, int height) {
    memcpy(pixels, blank_pixels, width * height * 4);

    uint32_t ball_colour = (((ball_x + ball_y) % 255) << BLUE_SHIFT) | dull_red | ALPHA;
    draw_circle(pixels, width, height, ball_x, ball_y, 10, ball_colour);
}

// IMPLEMENTS
void initialize(int width, int height, long micros_per_frame) {
    still_running = true;
    ball_x = width / 2;
    ball_y = height / 2;

    blank_pixels = (uint32_t*)malloc(height * width * 4);
    for (int i = 0; i < height * width; ++i) {
        blank_pixels[i] = ALPHA;
    }

    dull_red = 80 << RED_SHIFT;
}

// IMPLEMENTS
void process_frame_and_blit(long frame_count, long frame_time_in_micros, uint32_t *pixels, int width, int height) {
    ball_x += x_speed;
    ball_y += y_speed;

    blit(pixels, width, height);
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
    // Handle specific key presses from scancodes
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_RIGHT]){
        printf("right arrow key is pressed\n");
        x_speed += X_SPEED_CHANGE_RATE;
    }
    else if (state[SDL_SCANCODE_LEFT]){
        printf("left arrow key is pressed\n");
        x_speed -= X_SPEED_CHANGE_RATE;
    }
    if(state[SDL_SCANCODE_UP]){
        printf("up arrow key is pressed\n");
        y_speed -= Y_SPEED_CHANGE_RATE;
    }
    else if (state[SDL_SCANCODE_DOWN]){
        printf("down arrow key is pressed\n");
        y_speed += Y_SPEED_CHANGE_RATE;
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