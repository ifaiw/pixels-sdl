#ifndef _GAME_EDITOR__H
#define _GAME_EDITOR__H

#include "game_state.h"

// Returns error code, 0 for success
// slot can be 0-9, or 10 for autosave
int save_level(struct GameState* game_state, int slot);

int load_level(struct GameState* game_state, int slot);

#endif // _GAME_EDITOR__H