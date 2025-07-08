#include <assert.h>
#include <time.h>
#include <unistd.h>

// For printing out uints
#include <inttypes.h>

#include <iostream>

#include <SDL2/SDL.h>


// Our stuff
#include "graphics_constants.h"
#include "stuff_happens.h"


//#define WIDTH 1280
#define WIDTH 1500
#define HEIGHT 960
#define FRAMES_PER_SECOND 60
#define SLEEP_BUFFER_MICROS 500


static inline long to_micros(struct timespec *t1) {
    long micros_from_secs = t1->tv_sec * (long)1000000;
    long micros_from_nanos = t1->tv_nsec / (long)1000;
    return micros_from_secs + micros_from_nanos;
}


int main(int argc, char* argv[]){
    // Create a window data type
    // This pointer will point to the
    // window that is allocated from SDL_CreateWindow
    SDL_Window* window;

    // software render TODO STILL USED?
    // SDL_Surface* screen;

    // For hardware rendering TODO STILL USED?
    SDL_Texture* screen_texture;
    SDL_Renderer* sdl_renderer;

    // Initialize the video subsystem.
    // If it returns less than 1, then an
    // error code will be received.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized: %s\n", SDL_GetError());
    } else {
        printf("SDL video system is ready to go\n");
    }
    // Request a window to be created for our platform
    // The parameters are for the title, x and y position,
    // and the width and height of the window.
    window = SDL_CreateWindow("C++ SDL2 Window",
            20,
            20,
            WIDTH,
            HEIGHT,
            SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Error creating window!\n");
        exit(-1);
    }

    // hardware render
    sdl_renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    screen_texture = SDL_CreateTexture(
        sdl_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );
    if (screen_texture == NULL) {
        printf("Error creating screen texture!\n");
        SDL_DestroyWindow(window);
        exit(-1);
    }

    // software render TODO STILL USED?
    // screen = SDL_GetWindowSurface(window);

    uint32_t pixels[HEIGHT * WIDTH];

    // Main application loop
    struct timespec time_at_top;
    struct timespec time_at_bottom;
    struct timespec first_time_at_top;
    struct timespec time_start_render;
    struct timespec time_end_render;
    long time_at_top_micros;
    long time_at_bottom_micros;
    long first_micros_at_top;

    long micros_per_frame = (long)1000000 / (long)FRAMES_PER_SECOND;
    printf("micros_per_frame is %ld", micros_per_frame);
    long total_frame_count = 0;
    clock_gettime(CLOCK_REALTIME, &first_time_at_top);
    printf("first_time_at_top is %ld.%ld\n", first_time_at_top.tv_sec, first_time_at_top.tv_nsec);
    first_micros_at_top = to_micros(&first_time_at_top);
    printf("first_micros_at_top is %ld\n", first_micros_at_top);

    int initialize_result = initialize(WIDTH, HEIGHT, micros_per_frame);
    if (initialize_result != 0) {
        printf("ERROR in initialize: %d\n", initialize_result);
        fflush(stdout);
        exit(-1);
    }

    long target_time = first_micros_at_top;

    int PITCH = WIDTH * 32 / 8;
    // Apparently SDL_LockTexture sets the pitch parameter? Not sure why, but I don't think we want it changing PITCH
    int pitch_from_lock_texture = PITCH;

    // software render
    // process_frame_and_blit(0, first_micros_at_top, pixels, WIDTH, HEIGHT);
    // SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)pixels, WIDTH, HEIGHT, 32, WIDTH * 32 / 8, RED, GREEN, BLUE, ALPHA);
    // SDL_BlitSurface(surface,NULL,screen,NULL);
    // SDL_FreeSurface(surface);
    // SDL_UpdateWindowSurface(window);

    while(!should_stop()){
        clock_gettime(CLOCK_REALTIME, &time_at_top);
        time_at_top_micros = to_micros(&time_at_top);
        // printf("Current time %ld.%ld\n", time_at_top.tv_sec, time_at_top.tv_nsec);
        // printf("time_at_top_micros is %ld\n", time_at_top_micros);

        long target_miss = time_at_top_micros - target_time;
        // if (target_miss > 50) {
        //     printf("miss by %ld\n", target_miss);
        // }

        if (total_frame_count % 60 == 0) {
            // printf("Frame %ld at %ld\n", total_frame_count, time_at_top_micros);
        }
        SDL_Event event;
        // Start our event loop
        while(SDL_PollEvent(&event)){
            process_event(event);
        }

        process_frame_and_blit(total_frame_count, time_at_top_micros, pixels, WIDTH, HEIGHT);
        clock_gettime(CLOCK_REALTIME, &time_start_render);

        // software render
        // SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)pixels, WIDTH, HEIGHT, 32, WIDTH * 32 / 8, RED, GREEN, BLUE, ALPHA);
        // SDL_BlitSurface(surface,NULL,screen,NULL);
        // SDL_FreeSurface(surface);
        // SDL_UpdateWindowSurface(window);

        // hardware render
        // for testing SDL_UnlockTexture(screen_texture);
        SDL_UpdateTexture(screen_texture, NULL, pixels, PITCH);
        SDL_RenderCopy(sdl_renderer, screen_texture, NULL, NULL);
        SDL_RenderPresent(sdl_renderer);
        // for testing int lock_result = SDL_LockTexture(screen_texture, NULL, (void**)&pixels, &pitch_from_lock_texture);
        // for testing printf("SDL_LockTexture result is %d, pitch_from_lock_texture set to %d\n", lock_result, pitch_from_lock_texture);

        clock_gettime(CLOCK_REALTIME, &time_end_render);
        // printf("Render time: %ld\n", to_micros(&time_end_render) - to_micros(&time_start_render));
        total_frame_count++;

        clock_gettime(CLOCK_REALTIME, &time_at_bottom);
        time_at_bottom_micros = to_micros(&time_at_bottom);
        target_time = (total_frame_count * micros_per_frame) + first_micros_at_top;
        long micros_to_sleep = target_time - time_at_bottom_micros - SLEEP_BUFFER_MICROS;
        // printf("time elapsed in micros is %ld time to sleep in micros is %ld\n", (time_at_bottom_micros - time_at_top_micros), micros_to_sleep);
        if (micros_to_sleep > 0) {
           usleep(micros_to_sleep);
        } else {
            // printf("Frame %ld No time to sleep, already over by %ld!\n", total_frame_count, -micros_to_sleep);
        }
    }

    SDL_DestroyWindow(window);

    cleanup();

    // our program.
    SDL_Quit();
    return 0;
}
