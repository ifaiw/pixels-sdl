def gen_sprite(indent, sprite_name, image_index_name, x_offset, y_offset):
    l1 = f"{indent}r_sprite_array[{sprite_name}].height = BLOCK_HEIGHT_IN_PIXELS;"
    l2 = f"{indent}r_sprite_array[{sprite_name}].width = BLOCK_WIDTH_IN_PIXELS;"
    l3 = f"{indent}r_sprite_array[{sprite_name}].flip_left_to_right = false;"
    l4 = f"{indent}r_sprite_array[{sprite_name}].pixels_start = image_array[{image_index_name}].pixels + {x_offset} + {y_offset} * image_array[{image_index_name}].width;"
    l5 = f"{indent}r_sprite_array[{sprite_name}].image_source_pitch_in_pixels = image_array[{image_index_name}].width;"

    return [l1,l2,l3,l4,l5]

image_index_name = "IMAGE_INDEX_SOLIDS_2"

sprite_names = [
    "SPRITE_TYPE_GROUND_OPEN_LEFT",
    "SPRITE_TYPE_GROUND_OPEN_RIGHT",
    "SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT",
    "SPRITE_TYPE_GROUND_OPEN_UP",
    "SPRITE_TYPE_GROUND_OPEN_LEFT_UP",
    "SPRITE_TYPE_GROUND_OPEN_RIGHT_UP",
    "SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_UP",
    "SPRITE_TYPE_GROUND_OPEN_DOWN",
    "SPRITE_TYPE_GROUND_OPEN_LEFT_DOWN",
    "SPRITE_TYPE_GROUND_OPEN_RIGHT_DOWN",
    "SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_DOWN",
    "SPRITE_TYPE_GROUND_OPEN_UP_DOWN",
    "SPRITE_TYPE_GROUND_OPEN_LEFT_UP_DOWN",
    "SPRITE_TYPE_GROUND_OPEN_RIGHT_UP_DOWN",
    "SPRITE_TYPE_GROUND_OPEN_LEFT_RIGHT_UP_DOWN",
]
x_offsets = [
    192,
    192,
    192,
    192,
    96,
    144,
    192,
    192,
    192,
    192,
    192,
    192,
    192,
    192,
    192,
]

y_offsets = [
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
]

for i in range(0, len(sprite_names)):
    lines = gen_sprite("    ", sprite_names[i], image_index_name, x_offsets[i], y_offsets[i])
    for line in lines:
        print(line)
    print()
