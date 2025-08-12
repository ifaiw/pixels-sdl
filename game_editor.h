#ifndef _GAME_EDITOR__H
#define _GAME_EDITOR__H

#include "game_state.h"

// Returns error code, 0 for success
// slot can be 0-9, or 10 for autosave
int save_level(struct GameState* game_state, int slot);

int load_level(struct GameState* game_state, int slot);

void mouse_click(int in_game_x, int in_game_y_inverted, struct EditorState* r_editor_state, struct GameState* r_game_state);

#endif // _GAME_EDITOR__H