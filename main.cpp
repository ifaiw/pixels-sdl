#include <assert.h>
#include <time.h>
#include <unistd.h>

// For printing out uints
#include <inttypes.h>

#include <iostream>

#include <SDL2/SDL.h>

// From a separate library
#include <SDL_image.h>


// Our stuff
#include "graphics_constants.h"
#include "stuff_happens.h"


#define WIDTH 1280
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
    SDL_Window* window=nullptr;

    // Grab the window surface.
    SDL_Surface* screen;

    // Initialize the video subsystem.
    // If it returns less than 1, then an
    // error code will be received.
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "SDL could not be initialized: " <<
                  SDL_GetError();
    }else{
        std::cout << "SDL video system is ready to go\n";
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

    screen = SDL_GetWindowSurface(window);

    uint32_t pixels[HEIGHT * WIDTH];
    int x_offset = 0;
    int x_speed = 0;

    // Main application loop
    struct timespec time_at_top;
    struct timespec time_at_bottom;
    struct timespec first_time_at_top;
    long time_at_top_micros;
    long time_at_bottom_micros;
    long first_micros_at_top;

    // Not used anymore? long nanos_per_frame = (long)1000000000 / (long)FRAMES_PER_SECOND;
    // printf("nanos_per_frame is %ld\n", nanos_per_frame);
    long micros_per_frame = (long)1000000 / (long)FRAMES_PER_SECOND;
    printf("micros_per_frame is %ld", micros_per_frame);
    long total_frame_count = 0;
    clock_gettime(CLOCK_REALTIME, &first_time_at_top);
    printf("first_time_at_top is %ld.%ld\n", first_time_at_top.tv_sec, first_time_at_top.tv_nsec);
    first_micros_at_top = to_micros(&first_time_at_top);
    printf("first_micros_at_top is %ld\n", first_micros_at_top);

    initialize(WIDTH, HEIGHT, micros_per_frame);

    long target_time = first_micros_at_top;

    process_frame_and_blit(0, first_micros_at_top, pixels, WIDTH, HEIGHT);
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)pixels, WIDTH, HEIGHT, 32, WIDTH * 32 / 8, RED, GREEN, BLUE, ALPHA);
    SDL_BlitSurface(surface,NULL,screen,NULL);
    SDL_FreeSurface(surface);
    SDL_UpdateWindowSurface(window);

    while(!should_stop()){
        clock_gettime(CLOCK_REALTIME, &time_at_top);
        time_at_top_micros = to_micros(&time_at_top);
        printf("Current time %ld.%ld\n", time_at_top.tv_sec, time_at_top.tv_nsec);
        printf("time_at_top_micros is %ld\n", time_at_top_micros);

        long target_miss = time_at_top_micros - target_time;
        printf("miss by %ld\n", target_miss);

        if (total_frame_count % 60 == 0) {
            printf("Frame %ld at %ld\n", total_frame_count, time_at_top_micros);
        }
        SDL_Event event;
        // Start our event loop
        while(SDL_PollEvent(&event)){
            process_event(event);
        }

        process_frame_and_blit(0, first_micros_at_top, pixels, WIDTH, HEIGHT);
        SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)pixels, WIDTH, HEIGHT, 32, WIDTH * 32 / 8, RED, GREEN, BLUE, ALPHA);
        SDL_BlitSurface(surface,NULL,screen,NULL);
        SDL_FreeSurface(surface);
        SDL_UpdateWindowSurface(window);
 
        total_frame_count++;

        clock_gettime(CLOCK_REALTIME, &time_at_bottom);
        time_at_bottom_micros = to_micros(&time_at_bottom);
        target_time = (total_frame_count * micros_per_frame) + first_micros_at_top;
        long micros_to_sleep = target_time - time_at_bottom_micros - SLEEP_BUFFER_MICROS;
        printf("time elapsed in micros is %ld time to sleep in micros is %ld\n", (time_at_bottom_micros - time_at_top_micros), micros_to_sleep);
        if (micros_to_sleep > 0) {
           usleep(micros_to_sleep);
        }
    }

    // We destroy our window. We are passing in the pointer
    // that points to the memory allocated by the 
    // 'SDL_CreateWindow' function. Remember, this is
    // a 'C-style' API, we don't have destructors.
    SDL_DestroyWindow(window);

    cleanup();
    
    // our program.
    SDL_Quit();
    return 0;
}
