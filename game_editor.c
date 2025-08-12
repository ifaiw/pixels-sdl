#include "game_editor.h"

#include "game_paths.h"
#include "game_state.h"
#include "text_stuff.h"

// Returns error code, 0 for success
// slot can be 0-9, or 10 for autosave
// IMPLEMENTS
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

// IMPLEMENTS
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

// This method updates both r_editor_state and r_game_state
// Actually it doesn't currently change r_editor_state, but I think that will change in the future
// IMPLEMENTS
void mouse_click(int in_game_x, int in_game_y_inverted, struct EditorState* r_editor_state, struct GameState* r_game_state) {

    if (r_editor_state->click_state == ADD_BLOCK) {
        struct Block* mouse_block = get_world_block_for_world_pixel_xy(in_game_x, in_game_y_inverted, r_game_state);
        if (mouse_block != NULL) {
            // printf("clicked block effects_flag is %d\n", mouse_block->effects_flags);
            if (mouse_block->effects_flags & EFFECT_FLAG_SOLID || mouse_block->type == BLOCK_TYPE_LADDER) {
                printf("remove ground or ladder at %d,%d\n", mouse_block->block_x, mouse_block->block_y);
                mouse_block->type = BLOCK_TYPE_EMPTY;
                mouse_block->effects_flags = r_game_state->base_blocks[BLOCK_TYPE_EMPTY].effects_flags;
                mouse_block->sprite_index = r_game_state->base_blocks[BLOCK_TYPE_EMPTY].sprite_index;
            } else if (mouse_block->type == BLOCK_TYPE_EMPTY) {
                if (r_editor_state->block_type == BLOCK_TYPE_GROUND) {
                    printf("add ground at %d,%d\n", mouse_block->block_x, mouse_block->block_y);
                    mouse_block->type = BLOCK_TYPE_GROUND;
                    mouse_block->effects_flags = r_game_state->base_blocks[BLOCK_TYPE_GROUND].effects_flags;
                    mouse_block->sprite_index = r_game_state->base_blocks[BLOCK_TYPE_GROUND].sprite_index;
                }
                else if (r_editor_state->block_type == BLOCK_TYPE_LADDER) {
                    printf("add ladder at %d,%d\n", mouse_block->block_x, mouse_block->block_y);
                    mouse_block->type = BLOCK_TYPE_LADDER;
                    mouse_block->effects_flags = r_game_state->base_blocks[BLOCK_TYPE_LADDER].effects_flags;
                    mouse_block->sprite_index = r_game_state->base_blocks[BLOCK_TYPE_LADDER].sprite_index;
                }
                else if (r_editor_state->block_type == BLOCK_TYPE_TOILET) {
                    printf("add toilet at %d,%d\n", mouse_block->block_x, mouse_block->block_y);
                    mouse_block->type = BLOCK_TYPE_TOILET;
                    mouse_block->effects_flags = r_game_state->base_blocks[BLOCK_TYPE_TOILET].effects_flags;
                    mouse_block->sprite_index = r_game_state->base_blocks[BLOCK_TYPE_TOILET].sprite_index;
                }
            }
            update_ground_images(r_game_state);
        } else {
        // printf("No block at click\n");
        }
    }
    else {
        printf("add worm entity at %d,%d\n", in_game_x, in_game_y_inverted);
        struct Sprite* entity_sprite = &r_game_state->base_sprites[SPRITE_TYPE_WORM_CRAWL_1];

        int left = in_game_x - entity_sprite->width / 2 - 1;
        int right = in_game_x + entity_sprite->width/ 2 + 1;
        int top = in_game_y_inverted + entity_sprite->height / 2 + 1;
        int bottom = in_game_y_inverted - entity_sprite->height / 2 - 1;

        //TODO just for debugging
        // struct Block* block = get_world_block_for_world_pixel_xy(left, top, r_game_state);
        // printf("block to check is %d,%d effect flags %d\n", block->block_x, block->block_y, block->effects_flags);
        // block = get_world_block_for_world_pixel_xy(right, top, r_game_state);
        // printf("block to check is %d,%d effect flags %d\n", block->block_x, block->block_y, block->effects_flags);
        // block = get_world_block_for_world_pixel_xy(left, bottom, r_game_state);
        // printf("block to check is %d,%d effect flags %d\n", block->block_x, block->block_y, block->effects_flags);
        // block = get_world_block_for_world_pixel_xy(right, bottom, r_game_state);
        // printf("block to check is %d,%d effect flags %d\n", block->block_x, block->block_y, block->effects_flags);

        uint32_t entity_fits_in_blocks = !(
                (get_world_block_for_world_pixel_xy(left, top, r_game_state)->effects_flags & EFFECT_FLAG_SOLID)
            |   (get_world_block_for_world_pixel_xy(right, top, r_game_state)->effects_flags & EFFECT_FLAG_SOLID)
            |   (get_world_block_for_world_pixel_xy(left, bottom, r_game_state)->effects_flags & EFFECT_FLAG_SOLID)
            |   (get_world_block_for_world_pixel_xy(right, bottom, r_game_state)->effects_flags & EFFECT_FLAG_SOLID)
        );
        if (!entity_fits_in_blocks) {
            printf("Won't add entity because it doesn't fit\n");
            return;
        }

        uint16_t next_entity_index = get_next_free_entity_index(r_game_state);
        struct Entity* new_entity = r_game_state->entities + next_entity_index;
        new_entity->effects_flags = ENTITY_FLAG_IS_ACTIVE;

        new_entity->type = ENTITY_TYPE_WORM;

        new_entity->is_on_ground =
                (get_world_block_for_world_pixel_xy(left, bottom - 1, r_game_state)->effects_flags & EFFECT_FLAG_SOLID)
            |   (get_world_block_for_world_pixel_xy(right, bottom - 1, r_game_state)->effects_flags & EFFECT_FLAG_SOLID);
        if (new_entity->is_on_ground) {
            new_entity->state = ENTITY_STATE_MOVING;
            new_entity->x_velocity_pixels_per_second = r_game_state->world_rules.worm_x_speed_pixels_per_second;
        }
        else {
            new_entity->state = ENTITY_STATE_FALLING;
            new_entity->x_velocity_pixels_per_second = 0;
        }

        // Always start with gravity applied
        new_entity->y_velocity_pixels_per_second = -r_game_state->world_rules.gravity_pixels_per_second;

        new_entity->direction = RIGHT;
        new_entity->current_sprite = r_game_state->base_sprites[SPRITE_TYPE_WORM_CRAWL_1];
        new_entity->height = entity_sprite->height;
        new_entity->width = entity_sprite->width;
        new_entity->x_bottom_left = in_game_x - new_entity->width / 2;
        new_entity->y_inverted_bottom_left = in_game_y_inverted - new_entity->height / 2;
        new_entity->y_velocity_pixels_per_second = 0;
        new_entity->z_index = 5;
        new_entity->animation_time_start_in_micros = r_game_state->current_time_in_micros;
        //TODO set is_on_ground? remove it as an entity field?

    }

    // autosave level
    save_level(r_game_state, 10);

}