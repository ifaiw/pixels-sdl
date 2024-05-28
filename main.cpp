// On linux compile with:
// g++ -std=c++17 main.cpp -o prog -lSDL2 -ldl
// On windows compile with (if using mingw)
// g++ main.cpp -o prog.exe -lmingw32 -lSDL2main -lSDL2

#include <assert.h>

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

#define RED   0x00FF0000
#define GREEN 0x0000FF00
#define BLUE  0x000000FF
#define ALPHA 0xFF000000

// Assume 640x480
static void set_some_colours(uint32_t* pixels) {
    // Three horizontal colour bands
    int i = 0;
    for (int y = 0; y < 480; ++y) {
        for (int x = 0; x < 640; ++x) {
            if (y < 480/3) {
                pixels[i] = RED;
            }
            else if (y < 480 * 2 / 3) {
                pixels[i] = GREEN | ALPHA;
            }
            else {
                pixels[i] = BLUE | ALPHA;
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
            640,
            480,
            SDL_WINDOW_SHOWN);

    screen = SDL_GetWindowSurface(window);

    printf("About to load image\n");
    // SDL_Surface *image = IMG_Load("/Users/matthewenss/personal/c/assets/10x10-custom.png");

    //SDL_Surface* image;
//    image = SDL_LoadBMP("/Users/matthewenss/personal/c/assets/3D-Window_BBB_Blender_2.bmp");
    //image = SDL_LoadBMP("/Users/matthewenss/personal/c/assets/10x10-custom.bmp");
    //image = SDL_LoadBMP("/Users/matthewenss/personal/c/assets/10x10-custom-from-png.bmp");
    // image = SDL_LoadBMP("/Users/matthewenss/personal/c/assets/bmp-test.bmp");
    //printf("Image loaded\n");

    /*std::cout << "The pitch is " << image->pitch << "\n";
    std::cout << "width=" << image->w << " height=" << image->h << "\n";
    std::cout << "sizeof pixels is " << sizeof(image->pixels) << "\n";

    SDL_PixelFormat* format = image->format;
    printf("Bits per pixel is %" PRIu8 "\n", format->BitsPerPixel);
    printf("Bytes per pixel is %" PRIu8 "\n", format->BytesPerPixel);
    char s[49];
    printf("sizeof pixel format is %lu\n", sizeof(format->format));
    to_byte_str32(format->format, 0, s);
    printf("Format is %s\n", s);

    uint32_t* pixels = (uint32_t*)image->pixels;
    //for (int i = 0; i < (image->w * image->h); ++i) {
    for (int i = 0; i < 11; ++i) {
        printf("pixel %i is %" PRIu32 "\n", i, pixels[i]);
        s[48] = 0;
        to_byte_str32(pixels[i], 0, s);
        printf("hex is %s\n", s);
    }*/

    uint32_t pixels[480 * 640];
    set_some_colours(pixels);
     SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)pixels, 640, 480, 32, 640 * 32 / 8, RED, GREEN, BLUE, ALPHA);
    //SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)pixels, 20, 20, 32, 20, RED, GREEN, BLUE, ALPHA);
    printf("pixels[1] %" PRIu32 "\n", pixels[1]);
    printf("pixels[640*200] %" PRIu32 "\n", pixels[640 * 200]);
    printf("pixels[640*480-1] %" PRIu32 "\n", pixels[(640 * 480) - 1]);

    SDL_BlitSurface(surface,NULL,screen,NULL);
    SDL_FreeSurface(surface);
    SDL_UpdateWindowSurface(window);

    // Infinite loop for our application
    bool gameIsRunning = true;
    // Main application loop
    while(gameIsRunning){
        SDL_Event event;
        // Start our event loop
        while(SDL_PollEvent(&event)){
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
            }
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
