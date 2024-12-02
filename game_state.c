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
        printf("Unable to load world rules file $%s$ result %d\n", GAME_PATH__TEXT_PATH_WORLD_RULES_FULL, file_load_result);
        exit(-1);
    }
    printf("Below is the world_rules file\n%s\n", world_rules_file_bytes.bytes);
    struct TextKeyValueFileContents world_rules_file_dict;
    world_rules_file_dict.chars = (char*)world_rules_file_bytes.bytes;
    world_rules_file_dict.num_chars = world_rules_file_bytes.num_bytes;
    int key_value_parse_result = convert_file_to_key_values(&world_rules_file_dict);
    if (key_value_parse_result != 0) {
        printf("Unable to parse key/values from world rules file $%s$ result %d\n", GAME_PATH__TEXT_PATH_WORLD_RULES_FULL, file_load_result);
        exit(-1);
    }
    for (int i = 0; i < world_rules_file_dict.num_keys; ++i) {
        printf("Key: %s Value %s\n", world_rules_file_dict.chars + world_rules_file_dict.key_indices[i], world_rules_file_dict.chars + world_rules_file_dict.value_indices[i]);
    }

    r_world_rules->gravity_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"gravity_pixels_per_second", &world_rules_file_dict));
    r_world_rules->y_max_fall_speed_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"y_max_fall_speed_pixels_per_second", &world_rules_file_dict));

	r_world_rules->x_ground_acceleration_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"x_ground_acceleration_pixels_per_second", &world_rules_file_dict));
	r_world_rules->x_movement_max_speed_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"x_movement_max_speed_pixels_per_second", &world_rules_file_dict));

	r_world_rules->micros_per_walking_animation_frame = (long)chars_to_int(dict_get_value((char*)"micros_per_walking_animation_frame", &world_rules_file_dict));

    r_world_rules->y_jump_acceleration_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"y_jump_acceleration_pixels_per_second", &world_rules_file_dict));
	r_world_rules->microseconds_after_jump_start_check_jump_still_pressed = (double)chars_to_int(dict_get_value((char*)"microseconds_after_jump_start_check_jump_still_pressed", &world_rules_file_dict));

    free(world_rules_file_dict.chars);
    free(world_rules_file_dict.key_indices);
    free(world_rules_file_dict.value_indices);
}

// IMPLEMENTS
struct Block* get_world_block_for_location(int x, int y, struct GameState* game_state) {
    // TODO is x or y outside the world bounds handled properly? Just force the world to have a rectangle of blocks all around the outmost edge?
    if (x < 0 || y < 0) {
        return NULL;
    }

    int block_x = x / BLOCK_WIDTH_IN_PIXELS;
    int block_y = y / BLOCK_HEIGHT_IN_PIXELS;
    // printf("x=%d y=%d block_x=%d block_y=%d\n", x, y, block_x, block_y);
    if (block_x >= WIDTH_OF_WORLD_IN_BLOCKS || block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        return NULL;
    }

    return &game_state->world_blocks[block_y * WIDTH_OF_WORLD_IN_BLOCKS + block_x];
}

// Returns inverted y I think?
// IMPLEMENTS
struct XY get_bottom_left_world_pixel_for_block(struct Block* block) {
    struct XY xy;
    xy.x = block->world_x * BLOCK_WIDTH_IN_PIXELS;
    xy.y = block->world_y * BLOCK_HEIGHT_IN_PIXELS;
    return xy;
}

// IMPLEMENTS
bool is_on_ground(struct GameState* game_state) {
    printf("game_state.is_on_ground x_bottom_left=%f y_inverted_bottom_left=%f\n", game_state->character.x_bottom_left, game_state->character.y_inverted_bottom_left);
    int bottom_left_x_floor = floor(game_state->character.x_bottom_left);
    int bottom_left_y_floor = floor(game_state->character.y_inverted_bottom_left);
    int right_pixel = bottom_left_x_floor + game_state->character.width;
    int just_below_pixel = bottom_left_y_floor - 1;

    printf("y_inverted_bottom_left=%f bottom_left_y_floor=%d just_below_pixel=%d\n", game_state->character.y_inverted_bottom_left, bottom_left_y_floor, just_below_pixel);

    struct Block* bottom_left = get_world_block_for_location(bottom_left_x_floor, just_below_pixel, game_state);
    // TODO just for testing make the blocks below look different
    // bottom_left->sprite = game_state->base_sprites[SPRITE_TYPE_GROUND_TEST];
    printf("effects flag for bottom_left is %d\n", bottom_left->effects_flags);
    if (bottom_left->effects_flags & EFFECT_FLAG_SOLID) {
        return true;
    }
    struct Block* bottom_right = get_world_block_for_location(right_pixel, just_below_pixel, game_state);
    // TODO just for testing make the blocks below look different
    // bottom_right->sprite = game_state->base_sprites[SPRITE_TYPE_GROUND_TEST2];
    printf("effects flag for bottom_right is %d\n", bottom_right->effects_flags);
    return bottom_right->effects_flags & EFFECT_FLAG_SOLID;
}