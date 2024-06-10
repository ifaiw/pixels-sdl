#include <stdbool.h>

#include "stuff_happens.h"

#include "graphics_constants.h"

#define X_SPEED_CHANGE_RATE 2

static int x_offset = 0;
static int x_speed = 0;
static bool still_running = true;

// PRIVATE
void blit(uint32_t *pixels, int width, int height) {
    // Three horizontal colour bands
    int i = 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint32_t colour_level = ((x_offset + x) % 255);
            if (y < height/3) {
                pixels[i] = colour_level << RED_SHIFT | ALPHA;
            }
            else if (y < height * 2 / 3) {
                pixels[i] = colour_level << GREEN_SHIFT | ALPHA;
            }
            else {
                pixels[i] = colour_level << BLUE_SHIFT | ALPHA;
            }
            i += 1;
        }
    }
}

// IMPLEMENTS
void process_frame_and_blit(long frame_count, long frame_time_in_micros, uint32_t *pixels, int width, int height) {
    x_offset += x_speed;

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
}

// IMPLEMENTS
bool should_stop() {
    return !still_running;
}