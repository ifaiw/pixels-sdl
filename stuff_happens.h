#ifndef _STUFF_HAPPENS__H
#define _STUFF_HAPPENS__H

#include <inttypes.h>

#include <SDL2/SDL.h>

void process_frame_and_blit(long frame_count, long frame_time_in_micros, uint32_t *pixels, int width, int height);

void process_event(SDL_Event event);

bool should_stop();

#endif // _STUFF_HAPPENS__H