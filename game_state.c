#include "game_state.h"

#include <math.h>
// Just for printf
#include <stdio.h>

#include "game_paths.h"
#include "text_file_reader.h"
#include "utils.c"

// IMPLEMENTS
void load_world_rules_from_file(struct WorldRules* r_world_rules) {
    struct FileBytes world_rules_file_bytes;
    int file_load_result = read_file(GAME_PATH__TEXT_PATH_WORLD_RULES_FULL, &world_rules_file_bytes);
    if (file_load_result != 0) {
        // printf("Unable to load world rules file $%s$ result %d\n", GAME_PATH__TEXT_PATH_WORLD_RULES_FULL, file_load_result);
        exit(-1);
    }
    // printf("Below is the world_rules file\n%s\n", world_rules_file_bytes.bytes);
    struct TextKeyValueFileContents world_rules_file_dict;
    world_rules_file_dict.chars = (char*)world_rules_file_bytes.bytes;
    world_rules_file_dict.num_chars = world_rules_file_bytes.num_bytes;
    int key_value_parse_result = convert_file_to_key_values(&world_rules_file_dict);
    if (key_value_parse_result != 0) {
        // printf("Unable to parse key/values from world rules file $%s$ result %d\n", GAME_PATH__TEXT_PATH_WORLD_RULES_FULL, file_load_result);
        exit(-1);
    }
    for (int i = 0; i < world_rules_file_dict.num_keys; ++i) {
        // printf("Key: %s Value %s\n", world_rules_file_dict.chars + world_rules_file_dict.key_indices[i], world_rules_file_dict.chars + world_rules_file_dict.value_indices[i]);
    }

    r_world_rules->gravity_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"gravity_pixels_per_second", &world_rules_file_dict));
    r_world_rules->y_max_fall_speed_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"y_max_fall_speed_pixels_per_second", &world_rules_file_dict));

	r_world_rules->x_ground_acceleration_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"x_ground_acceleration_pixels_per_second", &world_rules_file_dict));
	r_world_rules->x_movement_max_speed_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"x_movement_max_speed_pixels_per_second", &world_rules_file_dict));
    r_world_rules->x_air_acceleration_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"x_air_acceleration_pixels_per_second", &world_rules_file_dict));

	r_world_rules->micros_per_walking_animation_frame = (long)chars_to_int(dict_get_value((char*)"micros_per_walking_animation_frame", &world_rules_file_dict));
    // TODO need chars_to_decimal, returns double
    r_world_rules->pixels_per_climbing_animation_frame = (double)chars_to_int(dict_get_value((char*)"micros_per_walking_animation_frame", &world_rules_file_dict));

    r_world_rules->y_jump_acceleration_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"y_jump_acceleration_pixels_per_second", &world_rules_file_dict));
	r_world_rules->microseconds_after_jump_start_check_jump_still_pressed = (double)chars_to_int(dict_get_value((char*)"microseconds_after_jump_start_check_jump_still_pressed", &world_rules_file_dict));

    r_world_rules->y_climb_speed_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"y_climb_speed_pixels_per_second", &world_rules_file_dict));
    r_world_rules->x_climb_speed_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"x_climb_speed_pixels_per_second", &world_rules_file_dict));

    free(world_rules_file_dict.chars);
    free(world_rules_file_dict.key_indices);
    free(world_rules_file_dict.value_indices);
}

// IMPLEMENTS
struct Block* get_world_block_for_location(int x, int y, struct GameState* game_state) {
    // printf("get_world_block_for_location x=%d y=%d\n", x, y);
    // TODO is x or y outside the world bounds handled properly? Just force the world to have a rectangle of blocks all around the outmost edge?
    if (x < 0 || y < 0) {
        // printf("get_world_block_for_location x or y is less than 0 return null\n");
        return NULL;
    }

    int block_x = x / BLOCK_WIDTH_IN_PIXELS;
    int block_y = y / BLOCK_HEIGHT_IN_PIXELS;
    // printf("get_world_block_for_location not null x=%d y=%d block_x=%d block_y=%d\n", x, y, block_x, block_y);
    if (block_x >= WIDTH_OF_WORLD_IN_BLOCKS || block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // printf("get_world_block_for_location x or y is too high return null\n");
        return NULL;
    }

    return &game_state->world_blocks[block_y * WIDTH_OF_WORLD_IN_BLOCKS + block_x];
}

// Returns inverted y I think?
// TODO Can delete this and change all calls to it to use world_pixel_ vars? I was just being lazy here
// IMPLEMENTS
struct XY get_bottom_left_world_pixel_for_block(struct Block* block) {
    // printf("block is %p\n", block);
    // fflush(stdout);
    // printf("get_bottom_left_world_pixel_for_block block->world_x=%d block->world_y=%d\n", block->world_x, block->world_y);
    // fflush(stdout);
    struct XY xy;
    // xy.x = block->world_x * BLOCK_WIDTH_IN_PIXELS;
    // xy.y = block->world_y * BLOCK_HEIGHT_IN_PIXELS;
    xy.x = block->world_pixel_x;
    xy.y = block->world_pixel_y;
    return xy;
}

// IMPLEMENTS
bool is_on_ground(struct GameState* game_state) {
    // printf("game_state.is_on_ground x_bottom_left=%f y_inverted_bottom_left=%f\n", game_state->character.x_bottom_left, game_state->character.y_inverted_bottom_left);
    int bottom_left_x_floor = floor(game_state->character.x_bottom_left);
    int bottom_left_y_floor = floor(game_state->character.y_inverted_bottom_left);
    int right_pixel = bottom_left_x_floor + game_state->character.width;
    int just_below_pixel = bottom_left_y_floor - 1;

    // printf("y_inverted_bottom_left=%f bottom_left_y_floor=%d just_below_pixel=%d\n", game_state->character.y_inverted_bottom_left, bottom_left_y_floor, just_below_pixel);

    struct Block* bottom_left = get_world_block_for_location(bottom_left_x_floor, just_below_pixel, game_state);
    if (bottom_left->effects_flags & EFFECT_FLAG_SOLID) {
        return true;
    }
    struct Block* bottom_right = get_world_block_for_location(right_pixel, just_below_pixel, game_state);
    return bottom_right->effects_flags & EFFECT_FLAG_SOLID;
}

// IMPLEMENTS
bool is_on_climable(struct GameState* game_state) {
    // printf("game_state.is_on_ground x_bottom_left=%f y_inverted_bottom_left=%f\n", game_state->character.x_bottom_left, game_state->character.y_inverted_bottom_left);
    int bottom_left_x_floor = floor(game_state->character.x_bottom_left);
    int bottom_left_y_floor = floor(game_state->character.y_inverted_bottom_left);
    int right_pixel = bottom_left_x_floor + game_state->character.width;

    struct Block* bottom_left = get_world_block_for_location(bottom_left_x_floor, bottom_left_y_floor, game_state);
    if ((bottom_left->effects_flags & EFFECT_FLAG_CLIMABLE) == 0) {
        return false;
    }
    struct Block* bottom_right = get_world_block_for_location(right_pixel, bottom_left_y_floor, game_state);
    // If character is straddling two different blocks then return false, even if both blocks are climable
    // Means that this method won't work for blocks that allow horizontal climbing, like lattice or web?
    if (bottom_left->block_x != bottom_right->block_x) {
        return false;
    }
    return true;
}

// IMPLEMENTS
void update_ground_images(struct GameState* game_state) {
    for (int block_x = 0; block_x < WIDTH_OF_WORLD_IN_BLOCKS; ++block_x) {
        for (int block_y = 0; block_y < HEIGHT_OF_WORLD_IN_BLOCKS; ++block_y) {
            struct Block* block = &(game_state->world_blocks[block_y * WIDTH_OF_WORLD_IN_BLOCKS + block_x]);
            if (block->type == BLOCK_TYPE_GROUND) {
                printf("updating ground sprite for %d,%d\n", block_x, block_y);
                int surrounding_block_mask = 0;
                if (block_x > 0 && game_state->world_blocks[block_y * WIDTH_OF_WORLD_IN_BLOCKS + block_x - 1].type != BLOCK_TYPE_GROUND) {
                    surrounding_block_mask += DIRECTION_MASK_LEFT;
                    printf("not ground on left\n");
                }
                if (block_x < WIDTH_OF_WORLD_IN_BLOCKS - 1 && game_state->world_blocks[block_y * WIDTH_OF_WORLD_IN_BLOCKS + block_x + 1].type != BLOCK_TYPE_GROUND) {
                    surrounding_block_mask += DIRECTION_MASK_RIGHT;
                    printf("not ground on right\n");
                }
                if (block_y > 0 && game_state->world_blocks[(block_y - 1) * WIDTH_OF_WORLD_IN_BLOCKS + block_x].type != BLOCK_TYPE_GROUND) {
                    surrounding_block_mask += DIRECTION_MASK_DOWN;
                    printf("not ground below\n");
                }
                if (block_y < HEIGHT_OF_WORLD_IN_BLOCKS - 1 && game_state->world_blocks[(block_y + 1) * WIDTH_OF_WORLD_IN_BLOCKS + block_x].type != BLOCK_TYPE_GROUND) {
                    surrounding_block_mask += DIRECTION_MASK_UP;
                    printf("not ground above\n");
                }

                int new_sprite_index = SPRITE_TYPE_GROUND_BLOCKED_ALL_SIDES + surrounding_block_mask;
                printf("Set Block %d,%d surrounding_block_mask=%d new_sprite_index=%d\n", block_x, block_y, surrounding_block_mask, new_sprite_index);

                block->sprite = game_state->base_sprites[new_sprite_index];
                printf("sprite_index of block->sprite is now %d\n", block->sprite.sprite_index);
            }
        }
    }
}
