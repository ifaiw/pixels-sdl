#ifndef _STUFF_HAPPENS__H
#define _STUFF_HAPPENS__H

#include <inttypes.h>

#include <SDL2/SDL.h>

// Return 0 if succeeded
int initialize(int width, int height, uint64_t micros_per_frame);

void process_frame_and_blit(uint64_t frame_count, uint64_t current_time_in_micros, uint32_t *pixels, int width, int height);

void process_event(SDL_Event event);

bool should_stop();

void cleanup();

#endif // _STUFF_HAPPENS__H