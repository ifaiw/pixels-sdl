#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "stuff_happens.h"

#include "graphics_constants.h"

#define X_SPEED_MAX_CHANGE_RATE 3
// Not currently used, multiple of gravity used instead
#define Y_SPEED_MAX_CHANGE_RATE 1

#define INITIAL_GRAVITY_PIXELS_PER_SECOND 22

#define NUM_BALLS 60

#define BALL_MIN_RADIUS 8
#define BALL_MAX_RADIUS 58
#define BALL_THICKNESS 5

#define X_BOUNDRY 10

#define INITIAL_FRAMES_TO_WAIT 180

static long micros_per_frame;

static double up_speed_from_hit = 10;
static double gravity_pixels_per_frame;
static double gravity_pixels_per_second = INITIAL_GRAVITY_PIXELS_PER_SECOND;
static double vertical_resistance_factor = 0.992;
static double horizontal_friction_factor = 0.96;

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

static struct Ball balls[NUM_BALLS];

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
        // printf("Illegal circle! %d %d %f", circle_x, circle_y, radius);
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
}

// IMPLEMENTS
void initialize(int width, int height, long micros_per_frame_param) {
    printf("top of initialize\n");
    still_running = true;
    srand(time(NULL));
    micros_per_frame = micros_per_frame_param;
    ground_y = height - 40;

    // TODO NEEDED? balls = (struct Ball*)malloc(NUM_BALLS * sizeof(struct Ball));
    for (int i = 0; i < NUM_BALLS; ++i) {
        printf("top of initialize balls loop\n");
        balls[i].radius = BALL_MIN_RADIUS + (rand() % (BALL_MAX_RADIUS - BALL_MIN_RADIUS));
        if (NUM_BALLS < width / 2) {
            balls[i].x = (width / NUM_BALLS * i) + (rand() % 5);
        } else {
            balls[i].x = rand() % (width - 2 * X_BOUNDRY - 2 * BALL_MAX_RADIUS);
        }
        balls[i].y = ground_y - (rand() % (height / 3 * 2));
        uint32_t red = (rand() % 255) << RED_SHIFT;
        uint32_t green = (rand() % 255) << GREEN_SHIFT;
        uint32_t blue = (rand() % 255) << BLUE_SHIFT;
        balls[i].colour = red | green | blue; // Currently adding alpha when drawing
        balls[i].thickness = BALL_THICKNESS;
        balls[i].x_speed = (rand() % 3) * ((rand() % 2) * 2 - 1);
        // balls[i].x_speed = 0;
        balls[i].y_speed = 0;
    }
    printf("done initialize balls loop\n");

    gravity_pixels_per_frame = (double)gravity_pixels_per_second / (double)1000000 * (double)micros_per_frame;
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
    blit(pixels, width, height);

    // Try skipping processing for the first second or so since framerate skips often happen at the very start
    if (frame_count > INITIAL_FRAMES_TO_WAIT) {
        for (int i = 0; i < NUM_BALLS; ++i) {
            bool is_on_ground = balls[i].y == ground_y - balls[i].radius;
            // printf("ttt top balls%d x_speed=%f y_speed=%f y=%f radius=%f\n", i, balls[i].x_speed, balls[i].y_speed, balls[i].y, balls[i].radius);
            balls[i].x += balls[i].x_speed;
            balls[i].y += balls[i].y_speed;

            if (!is_on_ground) {
                // printf("balls%d is not on the ground, yspeed=%f, so apply gravity\n", i, balls[i].y_speed);
                balls[i].y_speed += gravity_pixels_per_frame;
            }

            // if (i == 1) {
            //     printf("ball1 y=%f y_speed=%f\n", balls[i].y, balls[i].y_speed);
            // }

            // Deal with balls rolling on the ground
            if (is_on_ground && balls[i].y_speed == 0 && balls[i].x_speed != 0) {
                // printf("balls%d is rolling on the ground, apply friction\n", i);
                balls[i].x_speed = horizontal_friction_factor * balls[i].x_speed;
            }

            // if (balls[i].x_speed > 0 && balls[i].y_speed < 2 && balls[i].y < ground_y - balls[i].radius - 2) {
                // printf("ttt balls%d x_speed=%f y_speed=%f y=%f\n", i, balls[i].x_speed, balls[i].y_speed, balls[i].y);
            // }

            if (balls[i].y >= ground_y - balls[i].radius && balls[i].y_speed != 0) {
                // printf("\n\nballs%d bounces\n", i);
                balls[i].y_speed -= gravity_pixels_per_frame; // Subtract now to counter the increase in speed that just happened above, before we knew this ball was bouncing
                float bounce_speed_factor = -0.83;
                if (abs(balls[i].y_speed) < 3) {
                    bounce_speed_factor = - 0.5;
                }
                double speed_after_bounce = bounce_speed_factor * balls[i].y_speed;
                if (abs(speed_after_bounce) < 0.5) {
                    speed_after_bounce = 0;
                }
                // printf("Ball %d bouncing on the ground, speed going from %f to %f\n", i, balls[i].y_speed, speed_after_bounce);
                balls[i].y = ground_y - balls[i].radius;
                balls[i].y_speed = speed_after_bounce;
            }

            if (balls[i].x - balls[i].radius < X_BOUNDRY) {
                balls[i].x_speed = -0.9 * balls[i].x_speed;
                balls[i].x = balls[i].radius + X_BOUNDRY;
            }
            else if (balls[i].x + balls[i].radius > width - X_BOUNDRY) {
                balls[i].x_speed = -0.9 * balls[i].x_speed;
                balls[i].x = width - balls[i].radius - X_BOUNDRY;
            }

            balls[i].y_speed = vertical_resistance_factor * balls[i].y_speed;
        } // Main ball for loop
    } // if to skip processing on initial frames
}

// IMPLEMENTS
void process_event(SDL_Event event) {
    // printf("event type is %i\n", event.type);
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
        for (int i = 0; i < NUM_BALLS; ++i) {
            balls[i].x_speed += (rand() & X_SPEED_MAX_CHANGE_RATE);
        }
        // x_speed += X_SPEED_CHANGE_RATE;
    }
    else if (state[SDL_SCANCODE_LEFT]){
        printf("left arrow key is pressed\n");
        for (int i = 0; i < NUM_BALLS; ++i) {
            balls[i].x_speed -= (rand() & X_SPEED_MAX_CHANGE_RATE);
        }
        // x_speed -= X_SPEED_CHANGE_RATE;
    }
    if(state[SDL_SCANCODE_UP]){
        printf("up arrow key is pressed\n");
        for (int i = 0; i < NUM_BALLS; ++i) {
            // balls[i].y_speed = -4 - (rand() % 4);
            balls[i].y_speed = -(rand() % (1 * (int)gravity_pixels_per_second));
        }

        // gravity_pixels_per_second += 1;
        // gravity_pixels_per_frame = (double)gravity_pixels_per_second / (double)1000000 * (double)micros_per_frame;
    }
    else if (state[SDL_SCANCODE_DOWN]){
        printf("down arrow key is pressed, ignored for now\n");
        // gravity_pixels_per_second -= 1;
        // gravity_pixels_per_frame = (double)gravity_pixels_per_second / (double)1000000 * (double)micros_per_frame;
    }
}

// IMPLEMENTS
bool should_stop() {
    return !still_running;
}

// IMPLEMENTS
void cleanup() {
    free(blank_pixels);
    // TODO NEEDED? free(balls);
}