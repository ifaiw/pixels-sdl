#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "stuff_happens.h"

#include "graphics_constants.h"

#define X_SPEED_CHANGE_RATE 1
#define Y_SPEED_CHANGE_RATE 1

#define GRAVITY_PIXELS_PER_SECOND 22

#define NUM_BALLS 10

#define X_BOUNDRY 10

static double ball_x;
static double ball_y;
static int ball_radius;
static double x_speed = 0;
static double y_speed = 0;
static double up_speed_from_hit = 10;
static double gravity_pixels_per_frame;

static int ground_y;

static bool still_running;

static uint32_t *blank_pixels;

static uint32_t dull_red;

struct Ball {
    double radius;
    double x;
    double y;
    uint32_t colour;
    double thickness;
    double x_speed;
    double y_speed;
};

static struct Ball *balls;

// PRIVATE
inline void draw_circle(
    uint32_t *pixels,
    int window_width,
    int window_height, // width is currently used for row length, but height is not currently used
    int circle_x,
    int circle_y,
    double radius,
    uint32_t colour)
{
    if (circle_x - radius < 0 || circle_x + radius >= window_width || circle_y - radius < 0 || circle_y + radius >= window_height) {
        printf("Illegal circle! %d %d %f", circle_x, circle_y, radius);
        return;
    }

    int x = 0;
    int y = round(sqrt((double)(radius * radius - (double)(x * x))));
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
        y = round(sqrt((double)(radius * radius - (double)(x * x))));
    }
}

// PRIVATE
inline void blit(uint32_t *pixels, int width, int height) {
    memcpy(pixels, blank_pixels, width * height * 4);

    for (int i = 0; i < NUM_BALLS; ++i) {
        for (double r = balls[i].radius; r >= balls[i].radius - balls[i].thickness; r -= 0.5) {
            uint32_t alpha_mask = ALPHA;
            if (r == balls[i].radius || r == balls[i].radius - balls[i].thickness) {
                alpha_mask = HALF_ALPHA;
            }
            draw_circle(pixels, width, height, round(balls[i].x), round(balls[i].y), r, balls[i].colour | alpha_mask);
        }
    }

    // if (ball_y > ball_radius + 3) {
    //     // uint32_t ball_colour = (((int)round(ball_x + ball_y) % 255) << BLUE_SHIFT) | dull_red | ALPHA;
    //     uint32_t ball_colour = RED | ALPHA;
    //     for (double r = ball_radius; r >= ball_radius - 3; r -= 0.5) {
    //         draw_circle(pixels, width, height, round(ball_x), round(ball_y), r, ball_colour);
    //     }
    //     // draw_circle(pixels, width, height, round(ball_x), round(ball_y), ball_radius - 0.5, ball_colour);
    //     // draw_circle(pixels, width, height, round(ball_x), round(ball_y), ball_radius - 1, ball_colour);
    // }
}

// IMPLEMENTS
void initialize(int width, int height, long micros_per_frame) {
    printf("top of initialize\n");
    still_running = true;
    srand(time(NULL));
    ground_y = height - 40;

    balls = (struct Ball*)malloc(NUM_BALLS * sizeof(struct Ball));
    for (int i = 0; i < NUM_BALLS; ++i) {
        printf("top of initialize balls loop\n");
        balls[i].radius = 8 + (rand() % 40);
        balls[i].x = (width / NUM_BALLS * i) + (rand() % 5);
        balls[i].y = ground_y - (rand() % (height / 3 * 2));
        uint32_t red = (rand() % 255) << RED_SHIFT;
        uint32_t green = (rand() % 255) << GREEN_SHIFT;
        uint32_t blue = (rand() % 255) << BLUE_SHIFT;
        balls[i].colour = red | green | blue; // Currently adding alpha when drawing
        balls[i].thickness = 3;
        balls[i].x_speed = 0;
        balls[i].y_speed = 0;
    }
    printf("done initialize balls loop\n");

    ball_radius = 10;
    ball_x = width / 2;
    ball_y = ground_y - ball_radius;
    gravity_pixels_per_frame = (double)GRAVITY_PIXELS_PER_SECOND / (double)1000000 * (double)micros_per_frame;
    // 60 frames per second
    // fps / 1000000 = frames per micros
    // 1000000/fps = micros per frame
    // gravity per second / 1000000 = gravity per micros
    // gravity per micros * micros per frame = gravity per frame

    blank_pixels = (uint32_t*)malloc(height * width * 4);
    for (int i = 0; i < height * width; ++i) {
        blank_pixels[i] = ALPHA;
    }

    dull_red = 80 << RED_SHIFT;

    printf("bottom of initialize\n");
}

// IMPLEMENTS
void process_frame_and_blit(long frame_count, long frame_time_in_micros, uint32_t *pixels, int width, int height) {
    printf("top of process_frame_and_blit\n");
    for (int i = 0; i < NUM_BALLS; ++i) {
        balls[i].x += balls[i].x_speed;
        balls[i].y += balls[i].y_speed;
        balls[i].y_speed += gravity_pixels_per_frame;

        if (balls[i].y >= ground_y - balls[i].radius) {
            balls[i].y = ground_y - balls[i].radius;
            balls[i].y_speed = -balls[i].y_speed - 1.5 * gravity_pixels_per_frame;
        }

        if (balls[i].x - balls[i].radius < X_BOUNDRY) {
            balls[i].x_speed = -0.9 * balls[i].x_speed;
            balls[i].x = balls[i].radius + X_BOUNDRY;
        }
        else if (balls[i].x - balls[i].radius < X_BOUNDRY || balls[i].x + balls[i].radius > width - X_BOUNDRY) {
            balls[i].x_speed = -0.9 * balls[i].x_speed;
            balls[i].x = width - balls[i].radius - X_BOUNDRY;
        }
    }

    // ball_x += x_speed;
    // ball_y += y_speed;
    // y_speed += gravity_pixels_per_frame;

    // printf("ball_x=%f ball_y=%f y_speed=%f\n", ball_x, ball_y, y_speed);

    // if (ball_y >= ground_y - ball_radius) {
    //     ball_y = ground_y - ball_radius;
    //     printf("Hit the ground ball_y to %f\n", ball_y);
    //     y_speed = 0;
    // }

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
        // x_speed += X_SPEED_CHANGE_RATE;
    }
    else if (state[SDL_SCANCODE_LEFT]){
        printf("left arrow key is pressed\n");
        // x_speed -= X_SPEED_CHANGE_RATE;
    }
    if(state[SDL_SCANCODE_UP]){
        printf("up arrow key is pressed\n");
        // y_speed -= up_speed_from_hit;
    }
    else if (state[SDL_SCANCODE_DOWN]){
        printf("down arrow key is pressed, ignored for now\n");
        // y_speed += Y_SPEED_CHANGE_RATE;
    }
}

// IMPLEMENTS
bool should_stop() {
    return !still_running;
}

// IMPLEMENTS
void cleanup() {
    free(blank_pixels);
    free(balls);
}