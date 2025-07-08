#ifndef _GAME_EDITOR__H
#define _GAME_EDITOR__H

#include "game_paths.h"
#include "game_state.h"
#include "text_stuff.h"

// Returns error code, 0 for success
// slot can be 0-9, or 10 for autosave
const char SLOT_STRINGS[20] = {'0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0, '9', 0};
#define SLOT_STRING_AUTOSAVE "autosave"
int save_level(struct GameState* game_state, int slot) {
    const char* slot_string;
    if (slot >= 0 && slot < 10) {
        slot_string = SLOT_STRINGS + (slot * 2);
    }
    else {
        slot_string = SLOT_STRING_AUTOSAVE;
    }

    char* save_path = replace_once(GAME_PATH__LEVEL_FILES__EDITOR_FILES__FULL, slot_string);
    save_level_to_disk(game_state, save_path);

    return 0;
}

int load_level(struct GameState* game_state, int slot) {
    const char* slot_string;
    if (slot >= 0 && slot < 10) {
        slot_string = SLOT_STRINGS + (slot * 2);
    }
    else {
        slot_string = SLOT_STRING_AUTOSAVE;
    }

    char* load_path = replace_once(GAME_PATH__LEVEL_FILES__EDITOR_FILES__FULL, slot_string);
    load_level_from_disk(game_state, load_path);

    return 0;
}

#endif // _GAME_EDITOR__H