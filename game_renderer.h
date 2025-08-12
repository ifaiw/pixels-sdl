#ifndef _GAME_RENDERER__H
#define _GAME_RENDERER__H

#include <stdint.h>
#include <string.h>

#include "game_state.h"
#include "game_structs.h"
#include "rendering.h"

// PRIVATE
// TODO move this code into render/blit code?
static inline void update_sprites(struct GameState* game_state_param, struct InputState* input_state_param) {
    uint64_t microsecond_bucket;
    int walking_animation_frame_num;
    double pixel_distance_bucket;
    int climbing_animation_frame_num;

    switch (game_state_param->character.motion) {
        case STOPPED:
            // printf("motion is STOPPED show sprite STAND\n");
            game_state_param->character.current_sprite = game_state_param->base_sprites[game_state_param->character_sprite.stand_sprite_index];
            break;
        case CLIMBING:
            // Do 2 times y distance to make climbing up/down make the character animation move faster
            printf("climbing frame pixels_per_climbing_animation_frame=%f y_inverted_bottom_left=%f start_climb_pixel_y=%f x_bottom_left=%f start_climb_pixel_x=%f\n", game_state_param->world_rules.pixels_per_climbing_animation_frame, game_state_param->character.y_inverted_bottom_left, input_state_param->start_climb_pixel_y, game_state_param->character.x_bottom_left, input_state_param->start_climb_pixel_x);
            pixel_distance_bucket = (2 * fabs(game_state_param->character.y_inverted_bottom_left - input_state_param->start_climb_pixel_y) + fabs(game_state_param->character.x_bottom_left - input_state_param->start_climb_pixel_x)) / game_state_param->world_rules.pixels_per_climbing_animation_frame;

            // TODO casting to int here seems fishy
            climbing_animation_frame_num = (int)round(pixel_distance_bucket) % SPRITE_NUMBER_OF_FRAMES_MUSHROOM_CLIMB;
            printf("climbing frame pixel_distance_bucket=%f climbing_animation_frame_num=%d\n", pixel_distance_bucket, climbing_animation_frame_num);
            game_state_param->character.current_sprite = game_state_param->base_sprites[game_state_param->character_sprite.first_climb_sprite_index + climbing_animation_frame_num];
            break;
        case WALKING:
            microsecond_bucket = game_state_param->current_time_in_micros / game_state_param->world_rules.micros_per_walking_animation_frame;
            walking_animation_frame_num = microsecond_bucket % SPRITE_NUMBER_OF_FRAMES_MUSHROOM_WALK;
            // TODO just for testing
            // if (walking_animation_frame_num != last_walking_frame && walking_animation_frame_num != last_walking_frame+1 && !(last_walking_frame == 6 && walking_animation_frame_num == 0)) {
            //     printf("skipped walk animation frame, from %d to %d\n", last_walking_frame, walking_animation_frame_num);
            // }
            // last_walking_frame = walking_animation_frame_num;

            // printf("motion is WALKING show sprite WALK %d\n", walking_animation_frame_num);
            // printf("we're walking, current_time_in_micros=%ld microsecond_bucket=%ld walking_animation_frame_num=%d\n", game_state_param->current_time_in_micros, microsecond_bucket, walking_animation_frame_num);
            game_state_param->character.current_sprite = game_state_param->base_sprites[game_state_param->character_sprite.first_walk_sprite_index + walking_animation_frame_num];
            break;
        case JUMPING:
            // printf("motion is STOPPED show sprite WALK0\n");
            game_state_param->character.current_sprite = game_state_param->base_sprites[game_state_param->character_sprite.first_walk_sprite_index];
            break;
    }

// TODO Delete this code, updating sprites is handled in render code?
//     for (struct Entity* entity = game_state_param->entities; entity != game_state_param->entities + game_state_param->num_current_entites; ++entity) {
//         printf("Top of entity loop in update_sprites\n");
//         if (!(entity->effects_flags & ENTITY_FLAG_IS_ACTIVE)) {
//             printf("skip inactive entity\n");
//             continue;
//         }
//         if (    entity->x_bottom_left + entity->width < view_state->view_bottom_left_world_x
//             ||  entity->x_bottom_left > view_state->view_bottom_left_world_x + view_state->view_width
//             ||  entity->y_inverted_bottom_left + entity->height < view_state->view_bottom_left_world_y
//             ||  entity->y_inverted_bottom_left > view_state->view_bottom_left_world_y + view_state->view_height
//         ) {
//             printf("Skip entity not FULLY in view\n");
//             continue;
//         }
//     }

}

static inline void blit(uint32_t* r_pixels, struct GameState* game_state, struct ViewState* view_state, int width, int height) {
    // printf("top of stuff_happens_platformer.blit\n");

    // Clear pixel array to black
    memcpy(r_pixels, game_state->blank_pixels, width * height * 4);

    // Update view_state based on character position
    view_state->view_bottom_left_world_x = game_state->character.x_bottom_left + (game_state->character.width / 2) - (view_state->view_width / 2);
    view_state->view_bottom_left_world_y = game_state->character.y_inverted_bottom_left + (game_state->character.height / 2) - (view_state->view_height / 2);
    int view_top_right_world_x = view_state->view_bottom_left_world_x + view_state->view_width;
    int view_top_right_world_y = view_state->view_bottom_left_world_y + view_state->view_height;



    // TODO can delete
    // for (uint16_t i = 0; i < game_state->num_current_entites; ++i) {
    //     if (!(game_state->entities[i].effects_flags & ENTITY_FLAG_IS_ACTIVE) || game_state->entities[i].z_index != z_index) {
    //         continue;
    //     }
    //     if (    game_state->entities[i].x_bottom_left + game_state->entities[i].width < view_state->view_bottom_left_world_x
    //         ||  game_state->entities[i].x_bottom_left > view_state->view_bottom_left_world_x + view_state->view_width
    //         ||  game_state->entities[i].y_inverted_bottom_left + game_state->entities[i].)
    // }



    int left_block_x;
    if (view_state->view_bottom_left_world_x < 0) {
        left_block_x = (view_state->view_bottom_left_world_x / BLOCK_WIDTH_IN_PIXELS) - 1;
    } else {
        left_block_x = view_state->view_bottom_left_world_x / BLOCK_WIDTH_IN_PIXELS;
    }
    int left_block_x_left_pixel_x = left_block_x * BLOCK_WIDTH_IN_PIXELS;
    int right_block_x;
    if (view_top_right_world_x < 0) {
        right_block_x = (view_top_right_world_x / BLOCK_WIDTH_IN_PIXELS) - 1;
    } else {
        right_block_x = view_top_right_world_x / BLOCK_WIDTH_IN_PIXELS;
    }
    int right_block_x_left_pixel_x = right_block_x * BLOCK_WIDTH_IN_PIXELS;

    int bottom_block_y;
    if (view_state->view_bottom_left_world_y < 0) {
        bottom_block_y = (view_state->view_bottom_left_world_y / BLOCK_HEIGHT_IN_PIXELS) - 1;
    } else {
        bottom_block_y = view_state->view_bottom_left_world_y / BLOCK_HEIGHT_IN_PIXELS;
    }
    int bottom_block_y_bottom_pixel_y = bottom_block_y * BLOCK_HEIGHT_IN_PIXELS;
    int top_block_y;
    if (view_top_right_world_y < 0) {
        top_block_y = (view_top_right_world_y / BLOCK_HEIGHT_IN_PIXELS) - 1;
    } else {
        top_block_y = view_top_right_world_y / BLOCK_HEIGHT_IN_PIXELS;
    }
    int top_block_y_bottom_pixel_y = top_block_y * BLOCK_HEIGHT_IN_PIXELS;

    // printf("Calculatic top_block_num_y_rows_visible using view_state->view_bottom_left_world_y=%d view_state->view_height=%d top_block_y_bottom_pixel_y=%d\n", view_state->view_bottom_left_world_y, view_state->view_height, top_block_y_bottom_pixel_y);
    int top_block_num_y_rows_visible = view_state->view_bottom_left_world_y + view_state->view_height - top_block_y_bottom_pixel_y;
    int bottom_block_num_y_rows_visible = bottom_block_y_bottom_pixel_y + BLOCK_HEIGHT_IN_PIXELS - view_state->view_bottom_left_world_y;

    int left_block_num_x_cols_visible = left_block_x_left_pixel_x + BLOCK_WIDTH_IN_PIXELS - view_state->view_bottom_left_world_x;
    int right_block_num_x_cols_visible = view_state->view_bottom_left_world_x + view_state->view_width - right_block_x_left_pixel_x;

    // TODO clean up all the block drawing code below; different parts use different ways to reference the correct pixel on the screen
    // printf("blitting blocks in view\n");

    // Draw the full blocks in the middle
    for (int block_y_index = bottom_block_y + 1; block_y_index < top_block_y; ++block_y_index) {
        // printf("calculate top_left_y using view_state->view_area_offset_y=%d top_block_num_y_rows_visible=%d block_y_index=%d bottom_block_y=%d BLOCK_HEIGHT_IN_PIXELS=%d\n", view_state->view_area_offset_y, top_block_num_y_rows_visible, block_y_index, bottom_block_y, BLOCK_HEIGHT_IN_PIXELS);
        int top_left_y = view_state->view_area_offset_y + top_block_num_y_rows_visible + ((top_block_y - block_y_index - 1) * BLOCK_HEIGHT_IN_PIXELS);
        for (int block_x_index = left_block_x + 1; block_x_index < right_block_x; ++block_x_index) {
            // printf("top of inner loop block_x_index=%d block_y_index=%d\n", block_x_index, block_y_index);
            if (block_x_index < 0 || block_y_index < 0 || block_x_index >= WIDTH_OF_WORLD_IN_BLOCKS || block_y_index >= HEIGHT_OF_WORLD_IN_BLOCKS) {
                // printf("Skipping block for %d,%d because it doesn't exist\n", block_x_index, block_y_index);
                continue;
            }
            // printf("top left for block calculated using view_state->view_area_offset_x=%d left_block_num_x_cols_visible=%d block_x_index=%d left_block_x=%d BLOCK_WIDTH_IN_PIXELS=%d\n", view_state->view_area_offset_x, left_block_num_x_cols_visible, block_x_index, left_block_x, BLOCK_WIDTH_IN_PIXELS);
            int top_left_x = view_state->view_area_offset_x + left_block_num_x_cols_visible + ((block_x_index - left_block_x - 1) * BLOCK_WIDTH_IN_PIXELS);
            // TODO just for testing
            // int block_index = WIDTH_OF_WORLD_IN_BLOCKS * block_y_index + block_x_index;
            // int top_left_test_x = game_state->world_blocks[block_index].world_pixel_x - view_state->view_bottom_left_world_x + view_state->view_area_offset_x;
            // int world_diff_y = game_state->world_blocks[block_index].world_pixel_y - view_state->view_bottom_left_world_y;
            // int top_left_test_y = view_state->view_area_offset_y + view_state->view_height - world_diff_y - BLOCK_HEIGHT_IN_PIXELS;
            // printf("Using block pixel x and y, top-left on screen should be %d,%d\n", top_left_test_x, top_left_test_y);
            // TODO just for testing
            int sprite_index = game_state->world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * block_y_index + block_x_index].sprite_index;
            // printf("write block %d,%d [sprite_index=%d] at pixel top-left %d,%d\n", block_x_index, block_y_index, sprite_index, top_left_x, top_left_y);
            // fflush(stdout);
            write_sprite_aliased(   top_left_x,
                                    top_left_y,
                                    game_state->base_sprites[sprite_index],
                                    false,
                                    width,
                                    r_pixels);
            // printf("DONE write block %d,%d [sprite_index=%d] at pixel top-left %d,%d\n", block_x_index, block_y_index, sprite_index, top_left_x, top_left_y);
            // fflush(stdout);
        }
    }

    // printf("Done drawing full blocks in the middle, on to top-left\n");
    // fflush(stdout);

    // Draw top-left block
    if (left_block_x < 0 || top_block_y < 0 || left_block_x >= WIDTH_OF_WORLD_IN_BLOCKS || top_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // noop
        // printf("ttt Skip drawing top-left left_block_x=%d top_block_y=%d\n", left_block_x, top_block_y);
    } else {
        // printf("ttt do drawing top-left left_block_x=%d top_block_y=%d\n", left_block_x, top_block_y);
        write_sprite_aliased_subsection(    view_state->view_area_offset_x + left_block_num_x_cols_visible - BLOCK_WIDTH_IN_PIXELS,
                                            view_state->view_area_offset_y - BLOCK_HEIGHT_IN_PIXELS + top_block_num_y_rows_visible,
                                            game_state->base_sprites[game_state->world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * top_block_y + left_block_x].sprite_index],
                                            false, // flip_left_to_right
                                            BLOCK_WIDTH_IN_PIXELS - left_block_num_x_cols_visible,
                                            BLOCK_HEIGHT_IN_PIXELS - top_block_num_y_rows_visible,
                                            BLOCK_WIDTH_IN_PIXELS - 1,
                                            BLOCK_HEIGHT_IN_PIXELS - 1,
                                            width,
                                            r_pixels);
    }

    // printf("Done drawing top-left, on to top-right\n");
    // fflush(stdout);

    // Draw top-right block
    if (right_block_x < 0 || top_block_y < 0 || right_block_x >= WIDTH_OF_WORLD_IN_BLOCKS || top_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // noop
    } else {
    write_sprite_aliased_subsection(    view_state->view_area_offset_x + right_block_x_left_pixel_x - view_state->view_bottom_left_world_x,
                                        view_state->view_area_offset_y - BLOCK_HEIGHT_IN_PIXELS + top_block_num_y_rows_visible,
                                        game_state->base_sprites[game_state->world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * top_block_y + right_block_x].sprite_index],
                                        false, // flip_left_to_right
                                        0,
                                        BLOCK_HEIGHT_IN_PIXELS - top_block_num_y_rows_visible,
                                        right_block_num_x_cols_visible,
                                        BLOCK_HEIGHT_IN_PIXELS - 1,
                                        width,
                                        r_pixels);
    }

    // printf("Done drawing top-right, on to bottom-left\n");
    // fflush(stdout);

    // TODO just for testing
    int block_index = WIDTH_OF_WORLD_IN_BLOCKS * bottom_block_y + left_block_x;
    // printf("block_index for bottom left is %d\n", block_index);
    // fflush(stdout);
    // struct Block bottom_left_block = game_state->world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * bottom_block_y + left_block_x];
    // printf("For bottom-left block, view_state->view_bottom_left_world_x=%d view_state->view_bottom_left_world_y=%d bottom_left_block.world_pixel_x=%d bottom_left_block.world_pixel_y=%d\n", view_state->view_bottom_left_world_x, view_state->view_bottom_left_world_y, bottom_left_block.world_pixel_x, bottom_left_block.world_pixel_y);
    // fflush(stdout);
    // Draw bottom-left block
    if (left_block_x < 0 || bottom_block_y < 0 || left_block_x >= WIDTH_OF_WORLD_IN_BLOCKS || bottom_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // noop
    } else {
        // printf("about to write bottom-left\n");
        // fflush(stdout);
        write_sprite_aliased_subsection(    view_state->view_area_offset_x + left_block_num_x_cols_visible - BLOCK_WIDTH_IN_PIXELS,
                                            view_state->view_area_offset_y + view_state->view_height - bottom_block_num_y_rows_visible,
                                            game_state->base_sprites[game_state->world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * bottom_block_y + left_block_x].sprite_index],
                                            false, // flip_left_to_right
                                            BLOCK_WIDTH_IN_PIXELS - left_block_num_x_cols_visible,
                                            0,
                                            BLOCK_WIDTH_IN_PIXELS - 1,
                                            bottom_block_num_y_rows_visible,
                                            width,
                                            r_pixels);
    }

    // printf("Done drawing bottom-left, on to bottom-right\n");
    // fflush(stdout);

    // Draw bottom-right block
    if (right_block_x < 0 || bottom_block_y < 0 || right_block_x >= WIDTH_OF_WORLD_IN_BLOCKS || bottom_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // noop
    } else {
        write_sprite_aliased_subsection(    view_state->view_area_offset_x + view_state->view_width - right_block_num_x_cols_visible,
                                            view_state->view_area_offset_y + view_state->view_height - bottom_block_num_y_rows_visible,
                                            game_state->base_sprites[game_state->world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * bottom_block_y + right_block_x].sprite_index],
                                            false, // flip_left_to_right
                                            0,
                                            0,
                                            right_block_num_x_cols_visible,
                                            bottom_block_num_y_rows_visible,
                                            width,
                                            r_pixels);
    }

    // printf("bottom_block_num_y_rows_visible=%d bottom_block_y=%d\n", bottom_block_num_y_rows_visible, bottom_block_y);
    // fflush(stdout);
    // Draw left col of blocks
    if (left_block_x < 0 || left_block_x >= WIDTH_OF_WORLD_IN_BLOCKS) {
        // Noop
        // printf("ttt Skip drawing left-col left_block_x=%d top_block_y=%d\n", left_block_x, top_block_y);
    } else {
        // printf("ttt do drawing left-col left_block_x=%d top_block_y=%d\n", left_block_x, top_block_y);
        for (int block_y_index = 1; block_y_index < top_block_y - bottom_block_y; ++block_y_index) {
            // printf("Consider drawing left col box at block_y=%d\n", block_y_index + bottom_block_y);
            if (block_y_index + bottom_block_y < 0 || block_y_index + bottom_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
                continue;
            }
            // printf("Actually draw left col box at block_y=%d\n", block_y_index + bottom_block_y);
            write_sprite_aliased_subsection(    view_state->view_area_offset_x - BLOCK_WIDTH_IN_PIXELS + left_block_num_x_cols_visible,
                                                view_state->view_area_offset_y + view_state->view_height - bottom_block_num_y_rows_visible - (block_y_index * BLOCK_HEIGHT_IN_PIXELS),
                                                game_state->base_sprites[game_state->world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * (block_y_index + bottom_block_y) + left_block_x].sprite_index],
                                                false, // flip_left_to_right
                                                BLOCK_WIDTH_IN_PIXELS - left_block_num_x_cols_visible,
                                                0,
                                                BLOCK_WIDTH_IN_PIXELS - 1,
                                                BLOCK_HEIGHT_IN_PIXELS - 1,
                                                width,
                                                r_pixels);
        }
    }

    // Draw right col of blocks
    if (right_block_x < 0 || right_block_x >= WIDTH_OF_WORLD_IN_BLOCKS) {
        // Noop
    } else {
        for (int block_y_index = bottom_block_y + 1; block_y_index < top_block_y; ++block_y_index) {
            // printf("Consider drawing right col box at block_y=%d\n", block_y_index);
            if (block_y_index < 0 || block_y_index >= HEIGHT_OF_WORLD_IN_BLOCKS) {
                continue;
            }
            // printf("Actually draw right col box at block_y=%d\n", block_y_index);
            int world_block_index = WIDTH_OF_WORLD_IN_BLOCKS * block_y_index + right_block_x;
            write_sprite_aliased_subsection(    view_state->view_area_offset_x + right_block_x_left_pixel_x - view_state->view_bottom_left_world_x,
                                                view_state->view_area_offset_y + view_state->view_height - (game_state->world_blocks[world_block_index].world_pixel_y - view_state->view_bottom_left_world_y) - BLOCK_HEIGHT_IN_PIXELS,
                                                game_state->base_sprites[game_state->world_blocks[world_block_index].sprite_index],
                                                false, // flip_left_to_right
                                                0,
                                                0,
                                                right_block_num_x_cols_visible,
                                                BLOCK_HEIGHT_IN_PIXELS - 1,
                                                width,
                                                r_pixels);
        }
    }

    // Draw top row of blocks
    if (top_block_y < 0 || top_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // Noop
    } else {
        for (int block_x_index = left_block_x + 1; block_x_index < right_block_x; ++block_x_index) {
            // printf("ttt draw top-row block for x=%d left_block_num_x_cols_visible=%d\n", block_x_index, left_block_num_x_cols_visible);
            if (block_x_index < 0 || block_x_index >= WIDTH_OF_WORLD_IN_BLOCKS) {
                // printf("ttt skip draw top-row block for x=%d\n", block_x_index);
                continue;
            }
            write_sprite_aliased_subsection(   view_state->view_area_offset_x + left_block_num_x_cols_visible + ((block_x_index - left_block_x - 1) * BLOCK_WIDTH_IN_PIXELS),
                                    view_state->view_area_offset_y - BLOCK_HEIGHT_IN_PIXELS + top_block_num_y_rows_visible,
                                    game_state->base_sprites[game_state->world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * top_block_y + block_x_index].sprite_index],
                                    false, // flip_left_to_right
                                    0,
                                    BLOCK_HEIGHT_IN_PIXELS - top_block_num_y_rows_visible,
                                    BLOCK_WIDTH_IN_PIXELS - 1,
                                    BLOCK_HEIGHT_IN_PIXELS - 1,
                                    width,
                                    r_pixels);
        }
    }

    // Draw bottom row of blocks
    if (bottom_block_y < 0 || bottom_block_y >= HEIGHT_OF_WORLD_IN_BLOCKS) {
        // Noop
    } else {
        for (int block_x_index = 1; block_x_index < right_block_x - left_block_x; ++block_x_index) {
            if (block_x_index + left_block_x < 0 || block_x_index + left_block_x >= WIDTH_OF_WORLD_IN_BLOCKS) {
                continue;
            }
            // printf("Get world block for bottom_block_y=%d left_block_x=%d block_x_index=%d\n", bottom_block_y, left_block_x, block_x_index);
            write_sprite_aliased_subsection(   view_state->view_area_offset_x + left_block_num_x_cols_visible + ((block_x_index - 1) * BLOCK_WIDTH_IN_PIXELS),
                                    view_state->view_area_offset_y + view_state->view_height - bottom_block_num_y_rows_visible,
                                    game_state->base_sprites[game_state->world_blocks[WIDTH_OF_WORLD_IN_BLOCKS * bottom_block_y + left_block_x + block_x_index].sprite_index],
                                    false, // flip_left_to_right
                                    0,
                                    0,
                                    BLOCK_WIDTH_IN_PIXELS - 1,
                                    bottom_block_num_y_rows_visible,
                                    width,
                                    r_pixels);
        }
    }

    // Render entities
    // Currently the only z-index we use is 5
    int16_t z_index = 5;
    printf("Num entities is %d\n", game_state->num_current_entites);
    for (struct Entity* entity = game_state->entities; entity != game_state->entities + game_state->num_current_entites; ++entity) {
        printf("Top of entity loop in render\n");
        if (!(entity->effects_flags & ENTITY_FLAG_IS_ACTIVE) || entity->z_index != z_index) {
            printf("skip inactive or entity zindex=%d not at zindex %d\n", entity->z_index, z_index);
            continue;
        }
        if (    entity->x_bottom_left + entity->width < view_state->view_bottom_left_world_x
            ||  entity->x_bottom_left > view_state->view_bottom_left_world_x + view_state->view_width
            ||  entity->y_inverted_bottom_left + entity->height < view_state->view_bottom_left_world_y
            ||  entity->y_inverted_bottom_left > view_state->view_bottom_left_world_y + view_state->view_height
        ) {
            printf("Skip entity not FULLY in view\n");
            continue;
        }

        uint64_t micros_since_animation_start = game_state->current_time_in_micros - entity->animation_time_start_in_micros;
        uint16_t frame_num_worm = (uint16_t)(micros_since_animation_start / game_state->world_rules.worm_micros_per_walking_animation_frame) % SPRITE_NUMBER_OF_FRAMES_WORM_WALK;
        bool flip_entity_horizontal = false;
        switch (entity->type) {
            // entity->direction != RIGHT
            // No entity type flips the sprite, yet
            default:
                break;
        }

        write_sprite_aliased(   round(entity->x_bottom_left) - view_state->view_bottom_left_world_x + view_state->view_area_offset_x,
                                view_state->view_bottom_left_world_y
                                +   view_state->view_height
                                -   entity->y_inverted_bottom_left
                                +   view_state->view_area_offset_y
                                -   entity->height,
                                game_state->base_sprites[entity->current_sprite.sprite_index + frame_num_worm],
                                flip_entity_horizontal,
                                width,
                                r_pixels);
    }

    // printf("Done drawing blocks, next draw char\n");

    int char_left = round(game_state->character.x_bottom_left) - view_state->view_bottom_left_world_x + view_state->view_area_offset_x;
    int char_top =      view_state->view_bottom_left_world_y
                    +   view_state->view_height
                    -   game_state->character.y_inverted_bottom_left
                    +   view_state->view_area_offset_y
                    -   game_state->character.height;
    // printf("Draw character at %d,%d\n", char_left, char_top);
    write_sprite_aliased(char_left, char_top, game_state->character.current_sprite, game_state->character.direction == LEFT, width, r_pixels);

    //TODO just for testing
    // write_image(500, 200, game_state->base_bmp_images[IMAGE_INDEX_WORM], width, r_pixels);
}

#endif  // _GAME_RENDERER__H