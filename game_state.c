#include "game_state.h"

#include <math.h>
// Just for printf
#include <stdio.h>
#include <stdlib.h>
// Just for memcpy
#include <string.h>

#include "game_paths.h"
#include "file_stuff.h"
#include "utils.h"

// IMPLEMENTS
int16_t get_next_free_entity_index(struct GameState* game_state) {
    int16_t i;
    for (i = 0; i < game_state->num_current_entites; ++i) {
        if ((game_state->entities[i].effects_flags & ENTITY_FLAG_IS_ACTIVE) == 0) {
            return i;
        }
    }
    if (game_state->num_current_entites == TOTAL_POSSIBLE_ENTITIES) {
        printf("All possible entities in use!\n");
        return -1;
    }
    return game_state->num_current_entites++;
}

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

	r_world_rules->micros_per_walking_animation_frame = chars_to_uint32(dict_get_value((char*)"micros_per_walking_animation_frame", &world_rules_file_dict));
    // TODO need chars_to_decimal, returns double
    r_world_rules->pixels_per_climbing_animation_frame = (double)chars_to_int(dict_get_value((char*)"pixels_per_climbing_animation_frame", &world_rules_file_dict));

    r_world_rules->y_jump_acceleration_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"y_jump_acceleration_pixels_per_second", &world_rules_file_dict));
	r_world_rules->microseconds_after_jump_start_check_jump_still_pressed = (double)chars_to_int(dict_get_value((char*)"microseconds_after_jump_start_check_jump_still_pressed", &world_rules_file_dict));

    r_world_rules->y_climb_speed_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"y_climb_speed_pixels_per_second", &world_rules_file_dict));
    r_world_rules->x_climb_speed_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"x_climb_speed_pixels_per_second", &world_rules_file_dict));

    r_world_rules->worm_micros_per_walking_animation_frame = chars_to_uint32(dict_get_value((char*)"worm_micros_per_walking_animation_frame", &world_rules_file_dict));
    r_world_rules->worm_x_speed_pixels_per_second = (double)chars_to_int(dict_get_value((char*)"worm_x_speed_pixels_per_second", &world_rules_file_dict));


    free(world_rules_file_dict.chars);
    free(world_rules_file_dict.key_indices);
    free(world_rules_file_dict.value_indices);
}

// IMPLEMENTS
struct Block* get_world_block_for_world_pixel_xy(int pixel_x, int pixel_y, struct GameState* game_state) {
    // printf("get_world_block_for_world_pixel_xy x=%d y=%d\n", x, y);
    // TODO is x or y outside the world bounds handled properly? Just force the world to have a rectangle of blocks all around the outmost edge?
    if (pixel_x < 0 || pixel_y < 0) {
        // printf("get_world_block_for_world_pixel_xy x or y is less than 0 return null\n");
        return NULL;
    }

    int block_x = pixel_x / BLOCK_WIDTH_IN_PIXELS;
    int block_y = pixel_y / BLOCK_HEIGHT_IN_PIXELS;
    // printf("get_world_block_for_world_pixel_xy not null x=%d y=%d block_x=%d block_y=%d\n", x, y, block_x, block_y);
    if (block_x >= WIDTH_OF_WORLD_IN_BLOCKS || block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // printf("get_world_block_for_world_pixel_xy x or y is too high return null\n");
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

    struct Block* bottom_left = get_world_block_for_world_pixel_xy(bottom_left_x_floor, just_below_pixel, game_state);
    if (bottom_left->effects_flags & EFFECT_FLAG_SOLID) {
        return true;
    }
    struct Block* bottom_right = get_world_block_for_world_pixel_xy(right_pixel, just_below_pixel, game_state);
    return bottom_right->effects_flags & EFFECT_FLAG_SOLID;
}

// IMPLEMENTS
bool is_on_climable(struct GameState* game_state) {
    // printf("game_state.is_on_ground x_bottom_left=%f y_inverted_bottom_left=%f\n", game_state->character.x_bottom_left, game_state->character.y_inverted_bottom_left);
    int bottom_left_x_floor = floor(game_state->character.x_bottom_left);
    int bottom_left_y_floor = floor(game_state->character.y_inverted_bottom_left);
    int right_pixel = bottom_left_x_floor + game_state->character.width;

    struct Block* bottom_left = get_world_block_for_world_pixel_xy(bottom_left_x_floor, bottom_left_y_floor, game_state);
    if ((bottom_left->effects_flags & EFFECT_FLAG_CLIMABLE) == 0) {
        return false;
    }
    struct Block* bottom_right = get_world_block_for_world_pixel_xy(right_pixel, bottom_left_y_floor, game_state);
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
                // printf("updating ground sprite for %d,%d\n", block_x, block_y);
                int surrounding_block_mask = 0;
                if (block_x > 0 && game_state->world_blocks[block_y * WIDTH_OF_WORLD_IN_BLOCKS + block_x - 1].type != BLOCK_TYPE_GROUND) {
                    surrounding_block_mask += DIRECTION_MASK_LEFT;
                    // printf("not ground on left\n");
                }
                if (block_x < WIDTH_OF_WORLD_IN_BLOCKS - 1 && game_state->world_blocks[block_y * WIDTH_OF_WORLD_IN_BLOCKS + block_x + 1].type != BLOCK_TYPE_GROUND) {
                    surrounding_block_mask += DIRECTION_MASK_RIGHT;
                    // printf("not ground on right\n");
                }
                if (block_y > 0 && game_state->world_blocks[(block_y - 1) * WIDTH_OF_WORLD_IN_BLOCKS + block_x].type != BLOCK_TYPE_GROUND) {
                    surrounding_block_mask += DIRECTION_MASK_DOWN;
                    // printf("not ground below\n");
                }
                if (block_y < HEIGHT_OF_WORLD_IN_BLOCKS - 1 && game_state->world_blocks[(block_y + 1) * WIDTH_OF_WORLD_IN_BLOCKS + block_x].type != BLOCK_TYPE_GROUND) {
                    surrounding_block_mask += DIRECTION_MASK_UP;
                    // printf("not ground above\n");
                }

                int new_sprite_index = SPRITE_TYPE_GROUND_BLOCKED_ALL_SIDES + surrounding_block_mask;
                // printf("Set Block %d,%d index=%d surrounding_block_mask=%d new_sprite_index=%d\n", block_x, block_y, (block_y * WIDTH_OF_WORLD_IN_BLOCKS + block_x),surrounding_block_mask, new_sprite_index);

                block->sprite_index = new_sprite_index;
                // printf("sprite_index of block->sprite is now %d\n", block->sprite_index);
            }
        }
    }
}

// Implements
void populate_character_for_save_from_character(struct Character* character, struct CharacterForSave* r_character_for_save) {
    r_character_for_save->x_bottom_left = character->x_bottom_left;
    r_character_for_save->y_inverted_bottom_left = character->y_inverted_bottom_left;
    r_character_for_save->x_velocity_pixels_per_second = character->x_velocity_pixels_per_second;
    r_character_for_save->y_velocity_pixels_per_second = character->y_velocity_pixels_per_second;
    r_character_for_save->direction = character->direction;
    r_character_for_save->motion = character->motion;
}

// Implements
void populate_character_from_character_for_save(struct CharacterForSave* character_for_save, struct Character* r_character) {

    r_character->x_bottom_left = character_for_save->x_bottom_left;
    r_character->y_inverted_bottom_left = character_for_save->y_inverted_bottom_left;
    r_character->x_velocity_pixels_per_second = character_for_save->x_velocity_pixels_per_second;
    r_character->y_velocity_pixels_per_second = character_for_save->y_velocity_pixels_per_second;
    r_character->direction = character_for_save->direction;
    r_character->motion = character_for_save->motion;
}

void save_level_to_disk(struct GameState* game_state, const char* file_path) {

    struct LevelFileHeader header = {
        .filetype_name = LEVEL_FILE_HEADER_FILETYPE_NAME,
        .level_file_version = 1,
        .level_width_in_blocks = WIDTH_OF_WORLD_IN_BLOCKS,
        .level_height_in_blocks = HEIGHT_OF_WORLD_IN_BLOCKS
    };
    size_t header_size = sizeof(struct LevelFileHeader);
    printf("on save, header_size is %zu\n", header_size);

    populate_character_for_save_from_character(&game_state->character, &header.character_info);

    struct FileBytes level_file_bytes;
    uint32_t num_blocks = header.level_width_in_blocks * header.level_height_in_blocks;
    level_file_bytes.num_bytes = header_size + num_blocks * sizeof(struct Block);
    printf("on save, num_bytes is %lld\n", level_file_bytes.num_bytes);
    level_file_bytes.bytes = (uint8_t*)malloc(level_file_bytes.num_bytes);

    memcpy(level_file_bytes.bytes, &header, header_size);
    memcpy(level_file_bytes.bytes + header_size, game_state->world_blocks, num_blocks * sizeof(struct Block));
    write_file(file_path, &level_file_bytes, 0);

    free(level_file_bytes.bytes);
}

void print_level_file_header(struct LevelFileHeader* header) {
    printf("Level File Header\n");
    printf("filetype name: %s\n", header->filetype_name);
    printf("file version: %d\n", header->level_file_version);
    printf("width: %d\n", header->level_width_in_blocks);
    printf("height: %d\n", header->level_height_in_blocks);
}

void load_level_from_disk(struct GameState* r_game_state, const char* file_path) {

    struct FileBytes level_file_bytes;
    int result = read_file(file_path, &level_file_bytes);
    if (result != 0) {
        printf("Error reading file %s: %d\n", file_path, result);
        exit(-1);
    }

    // TODO confirm header and version
    // Maybe that means the header should only be filetype_name and version, not width or height?

    struct LevelFileHeader header = {
        .filetype_name = LEVEL_FILE_HEADER_FILETYPE_NAME,
        .level_file_version = 1,
        .level_width_in_blocks = WIDTH_OF_WORLD_IN_BLOCKS,
        .level_height_in_blocks = HEIGHT_OF_WORLD_IN_BLOCKS
    };
    size_t header_size = sizeof(struct LevelFileHeader);
    printf("on load, header_size is %zu\n", header_size);

    memcpy(&header, level_file_bytes.bytes, header_size);
    print_level_file_header(&header);
    uint32_t num_blocks_in_level = header.level_height_in_blocks * header.level_width_in_blocks;
    size_t bytes_for_level_blocks = sizeof(struct Block) * num_blocks_in_level;
    printf("bytes_for_level_blocks is %zu\n", bytes_for_level_blocks);
    // TODO sanity check we're not going past level_files_bytes.num_bytes?
    memcpy(r_game_state->world_blocks, level_file_bytes.bytes + header_size, bytes_for_level_blocks);

    populate_character_from_character_for_save(&header.character_info, &r_game_state->character);

    free(level_file_bytes.bytes);
}