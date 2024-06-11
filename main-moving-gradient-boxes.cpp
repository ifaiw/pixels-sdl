// On linux compile with:
// g++ -std=c++17 main.cpp -o prog -lSDL2 -ldl
// On windows compile with (if using mingw)
// g++ main.cpp -o prog.exe -lmingw32 -lSDL2main -lSDL2

#include <assert.h>
#include <time.h>
#include <unistd.h>

// For printing out uints
#include <inttypes.h>

#include <iostream>

#include <SDL2/SDL.h>

// From a separate library
#include <SDL_image.h>


// 
static void to_byte_str(uint8_t b, int buffer_index, char* o_buffer) {
    for (int i = 15; i >= 0; --i) {
        uint8_t next_byte = b & 0xf;
        assert(next_byte < 16);
        if (next_byte < 9) {
            o_buffer[buffer_index + i] = '0' + next_byte;
        } else {
            o_buffer[buffer_index + i] = 'A' - 10 + next_byte;
        }
        // printf("next_byte is %" PRIu8 "\n", next_byte);
        b = b >> 4;
    }
}

static void to_byte_str32(uint32_t b, int buffer_index, char* o_buffer) {
    for (int i = 47; i >= 0; --i) {
        uint32_t next_byte = b & 0xf;
        assert(next_byte < 16);
        if (next_byte < 9) {
            o_buffer[buffer_index + i] = '0' + next_byte;
        } else {
            o_buffer[buffer_index + i] = 'A' - 10 + next_byte;
        }
        b = b >> 4;
    }
}

#define WIDTH 1280
#define HEIGHT 960
#define FRAMES_PER_SECOND 60
#define SLEEP_BUFFER_MICROS 1000
#define X_KEYPRESS_SPEED 15
#define RED   0x00FF0000
#define GREEN 0x0000FF00
#define BLUE  0x000000FF
#define ALPHA 0xFF000000
#define RED_SHIFT   16
#define GREEN_SHIFT  8
#define BLUE_SHIFT   0

static inline long diff_time_in_nanos(struct timespec *t1, struct timespec *t2) {
    long diff_in_nanos_from_sec = (t2->tv_sec - t1->tv_sec) * (long)1000000000;
    long diff_in_nanos_from_nanos = t2->tv_nsec - t1->tv_nsec;
    return diff_in_nanos_from_sec + diff_in_nanos_from_nanos;
}

static inline long diff_time_in_micros(struct timespec *t1, long t2_secs, long t2_nanos) {
    long diff_in_micros_from_sec = (t2_secs - t1->tv_sec) * (long)1000000;
    long diff_in_micros_from_nanos = (t2_nanos - t1->tv_nsec) / (long)1000;
    return diff_in_micros_from_sec + diff_in_micros_from_nanos;
}

static inline long micros_until_next_frame(struct timespec *initial, struct timespec *current, long next_frame_count, long nanos_per_frame) {
    long nanos_of_next_frame_since_initial = next_frame_count * nanos_per_frame;
    long seconds_of_next_frame_since_initial = nanos_of_next_frame_since_initial / (long)1000000000;
    long nanos_remainder = nanos_of_next_frame_since_initial - (seconds_of_next_frame_since_initial * (long)1000000000);

    long seconds_next_frame = initial->tv_sec + seconds_of_next_frame_since_initial;
    long nanos_next_frame = initial->tv_nsec + nanos_remainder;

    return diff_time_in_micros(current, seconds_next_frame, nanos_next_frame);
}

static inline long micros_until_next_frame(long initial_micros, long current_micros, long next_frame_count, long micros_per_frame) {
    long micros_of_next_frame_since_initial = next_frame_count * micros_per_frame;
    long micros_of_next_frame = micros_of_next_frame_since_initial + initial_micros;
    return micros_of_next_frame - current_micros;
}

static inline long to_micros(struct timespec *t1) {
    long micros_from_secs = t1->tv_sec * (long)1000000;
    long micros_from_nanos = t1->tv_nsec / (long)1000;
    return micros_from_secs + micros_from_nanos;
}


static void set_some_colours(uint32_t* pixels, int width, int height, int x_offset) {
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

    set_some_colours(pixels, WIDTH, HEIGHT, x_offset);
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)pixels, WIDTH, HEIGHT, 32, WIDTH * 32 / 8, RED, GREEN, BLUE, ALPHA);
    SDL_BlitSurface(surface,NULL,screen,NULL);
    SDL_FreeSurface(surface);
    SDL_UpdateWindowSurface(window);

    // Infinite loop for our application
    bool gameIsRunning = true;
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

    long target_time = first_micros_at_top;

    while(gameIsRunning){
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
            printf("event type is %i\n", event.type);
            // Handle each specific event
            if(event.type == SDL_QUIT){
                gameIsRunning= false;
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
                std::cout << "right arrow key is pressed\n";
                x_speed += 2;
            }
            else if (state[SDL_SCANCODE_LEFT]){
                std::cout << "left arrow key is pressed\n";
                x_speed -= 2;
            }
        }

        x_offset += x_speed;

        set_some_colours(pixels, WIDTH, HEIGHT, x_offset);
        SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)pixels, WIDTH, HEIGHT, 32, WIDTH * 32 / 8, RED, GREEN, BLUE, ALPHA);
        SDL_BlitSurface(surface,NULL,screen,NULL);
        SDL_FreeSurface(surface);
        SDL_UpdateWindowSurface(window);
 
        total_frame_count++;

        clock_gettime(CLOCK_REALTIME, &time_at_bottom);
        time_at_bottom_micros = to_micros(&time_at_bottom);
        //long micros_to_sleep = micros_until_next_frame(first_micros_at_top, time_at_bottom_micros, total_frame_count, micros_per_frame);
        target_time = (total_frame_count * micros_per_frame) + first_micros_at_top;
        long micros_to_sleep = target_time - time_at_bottom_micros - SLEEP_BUFFER_MICROS;
        // long nanos_elapsed = diff_time(&time_at_top, &time_at_bottom);
        // long nanos_to_sleep = (nanos_per_frame - nanos_elapsed - SLEEP_BUFFER);
        // printf("time elapsed in nanos is %ld time to sleep in nanos is %ld\n", nanos_elapsed, nanos_to_sleep);
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
    
    // our program.
    SDL_Quit();
    return 0;
}
