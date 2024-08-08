#include <assert.h>
#include <time.h>
#include <unistd.h>

// For printing out uints
#include <inttypes.h>

#include <iostream>

#include <SDL2/SDL.h>


// Our stuff
#include "graphics_constants.h"
#include "image_bmp_loader.h"
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


int main(int argc, char* argv[]) {
    struct Image_info image = {-1, -1, NULL};
    // char const *path = "/Users/matthew.enss/personal/c/assets/tilesets/FreeCuteTileset/Decors.png";
    // char const *path = "/Users/matthew.enss/personal/c/assets/tilesets/FreeCuteTileset/Decors-three-pixels-top-left.png";
    // char const *path = "/Users/matthew.enss/personal/c/assets/test-3x3b.bmp";
    // char const *path = "/Users/matthew.enss/personal/c/assets/tilesets/FreeCuteTileset/Decors.bmp";
    char const *path = "/Users/matthew.enss/personal/c/assets/tilesets/FreeCuteTileset/Tileset.bmp";
    // __FREE_REQUIRED
    int result = load_bmp_image(path, &image);
    printf("bmp result is %d\n", result);

    // uint8_t* pixel_bytes = (uint8_t*)image.pixels;

    // printf("all the bytes\n");
    // for (int i = 0; i < 30; ++i) {
    //     printf("%02x", pixel_bytes[i]);
    // }
    // printf("\n");
    // printf("that's all the bytes\n");
    // exit(0);
    // printf("First 5 pixels as ints\n");
    // for (int i = 0; i < 5; ++i) {
    //     printf("%d\n", image.pixels[i]);
    // }
    // printf("First 20 pixels\n");
    // int y = 0;
    // for (int i = 0; i < 20; ++i) {
    //     printf("%x ", image.pixels[(y * image.width) + i]);
    // }
    // printf("\n");
    // printf("Pixels 20 to 40 on a few lines\n");
    // y = 0;
    // for (int i = 20; i < 40; ++i) {
    //     printf("%x ", image.pixels[(y * image.width) + i]);
    // }
    // printf("\n");
    // y = 20;
    // for (int i = 20; i < 40; ++i) {
    //     printf("%x ", image.pixels[(y * image.width) + i]);
    // }
    // printf("\n");
    // y = 30;
    // for (int i = 20; i < 40; ++i) {
    //     printf("%x ", image.pixels[(y * image.width) + i]);
    // }
    // printf("\n");


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
        // original format SDL_PIXELFORMAT_RGBA32,
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

    // TODO just for testing
    // SDL_UpdateTexture(screen_texture, NULL, image.pixels, image.width * 4);
    // SDL_RenderCopy(sdl_renderer, screen_texture, NULL, NULL);
    // SDL_RenderPresent(sdl_renderer);
    // usleep(10000000);
    // exit(0);


    // software render TODO STILL USED?
    // screen = SDL_GetWindowSurface(window);

    uint32_t pixels[HEIGHT * WIDTH];
    memset(pixels, ALPHA, HEIGHT * WIDTH * 4);

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

    // initialize(WIDTH, HEIGHT, micros_per_frame);

    long target_time = first_micros_at_top;

    // software render
    // process_frame_and_blit(0, first_micros_at_top, pixels, WIDTH, HEIGHT);
    // SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)pixels, WIDTH, HEIGHT, 32, WIDTH * 32 / 8, RED, GREEN, BLUE, ALPHA);
    // SDL_BlitSurface(surface,NULL,screen,NULL);
    // SDL_FreeSurface(surface);
    // SDL_UpdateWindowSurface(window);

    // TODO just for testing while(!should_stop()){
    bool should_stop = false;
    while(!should_stop) {
        clock_gettime(CLOCK_REALTIME, &time_at_top);
        time_at_top_micros = to_micros(&time_at_top);
        // printf("Current time %ld.%ld\n", time_at_top.tv_sec, time_at_top.tv_nsec);
        // printf("time_at_top_micros is %ld\n", time_at_top_micros);

        long target_miss = time_at_top_micros - target_time;
        // if (target_miss > 50) {
        //     printf("miss by %ld\n", target_miss);
        // }

        if (total_frame_count % 60 == 0) {
            printf("Frame %ld at %ld\n", total_frame_count, time_at_top_micros);
        }
        SDL_Event event;
        // Start our event loop
        while (SDL_PollEvent(&event)) {
            // TODO just for testing process_event(event);
            if (event.type == SDL_QUIT) {
                should_stop = true;
            }
        }

        // TODO just for testing process_frame_and_blit(total_frame_count, first_micros_at_top, pixels, WIDTH, HEIGHT);
        for (int y = 0; y < image.height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                pixels[y * WIDTH + x] = image.pixels[(image.height - y) * image.width + x];
            }
        }

        clock_gettime(CLOCK_REALTIME, &time_start_render);

        // software render
        // SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)pixels, WIDTH, HEIGHT, 32, WIDTH * 32 / 8, RED, GREEN, BLUE, ALPHA);
        // SDL_BlitSurface(surface,NULL,screen,NULL);
        // SDL_FreeSurface(surface);
        // SDL_UpdateWindowSurface(window);

        // hardware render
        SDL_UpdateTexture(screen_texture, NULL, pixels, WIDTH * 32 / 8);
        SDL_RenderCopy(sdl_renderer, screen_texture, NULL, NULL);
        SDL_RenderPresent(sdl_renderer);

        clock_gettime(CLOCK_REALTIME, &time_end_render);
        printf("Render time: %ld\n", to_micros(&time_end_render) - to_micros(&time_start_render));
        total_frame_count++;

        clock_gettime(CLOCK_REALTIME, &time_at_bottom);
        time_at_bottom_micros = to_micros(&time_at_bottom);
        target_time = (total_frame_count * micros_per_frame) + first_micros_at_top;
        long micros_to_sleep = target_time - time_at_bottom_micros - SLEEP_BUFFER_MICROS;
        // printf("time elapsed in micros is %ld time to sleep in micros is %ld\n", (time_at_bottom_micros - time_at_top_micros), micros_to_sleep);
        if (micros_to_sleep > 0) {
           usleep(micros_to_sleep);
        } else {
            printf("Frame %ld No time to sleep, already over by %ld!\n", total_frame_count, -micros_to_sleep);
        }
    }

    // We destroy our window. We are passing in the pointer
    // that points to the memory allocated by the
    // 'SDL_CreateWindow' function. Remember, this is
    // a 'C-style' API, we don't have destructors.
    SDL_DestroyWindow(window);

    // TODO just for testing cleanup();

    // our program.
    SDL_Quit();
    return 0;
}
