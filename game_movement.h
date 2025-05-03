#ifndef _GAME_MOVEMENT__H
#define _GAME_MOVEMENT__H

#include "game_structs.h"


void do_movement(struct GameState* game_state, double microseconds_to_advance);

void handle_input(  struct GameState* game_state,
                    struct ViewState* view_state,
                    struct InputState* input_state,
                    struct EditorState* editor_state,
                    double microseconds_to_advance);

#endif  // _GAME_MOVEMENT__H