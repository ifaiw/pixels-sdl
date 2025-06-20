#include "file_stuff.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// IMPLEMENTS
// __FREE_REQUIRED for r_file_bytes.bytes
int read_file(const char* file_path, struct FileBytes* r_file_bytes) {
    FILE* file_ptr;

    file_ptr = fopen(file_path, "rb");
    if (file_ptr == NULL) {
       printf("Error opening file\n");
       return -1;
    }

    uint64_t length;
    fseek(file_ptr, 0, SEEK_END);
    length = ftell(file_ptr);
    rewind(file_ptr);

    printf("file length is %lld\n", length);

    uint8_t* buffer = (uint8_t*)malloc((length + 1) * sizeof(uint8_t)); // length+1 to add our own null terminator
    uint64_t bytes_read = length * fread(buffer, length, 1, file_ptr);
    if (bytes_read != length) {
       printf("bytes_read differs from length: %lld, %lld\n", bytes_read, length);
       return -1;
    }
    fclose(file_ptr);

    // TODO Can delete?
    // if (buffer[length-1] != 0) {
    //     printf("File buffer is not null-terminated: %hhx\n", buffer[length-1]);
    //     return -2;
    // }

    buffer[length] = 0;

    r_file_bytes->bytes = buffer;
    r_file_bytes->num_bytes = length + 1;
    return 0;
}

// IMPLEMENTS
// fail_if_exists == "true" if not 0
int write_file(const char* file_path, struct FileBytes* file_bytes, uint8_t fail_if_exists) {
    FILE* file_ptr;

    if (fail_if_exists != 0) {
        file_ptr = fopen(file_path, "r");
        if (file_ptr != NULL) {
            printf("Error writing to file that already exists: %s\n", file_path);
            return -1;
        }
        fclose(file_ptr);
    }

    file_ptr = fopen(file_path, "w");
    if (file_ptr == NULL) {
        printf("Error opening file to write to: %s\n", file_path);
        return -2;
    }

    fwrite(file_bytes->bytes, sizeof(uint8_t), file_bytes->num_bytes, file_ptr);

    fclose(file_ptr);
    return 0;
}

#define TEXT_FILE_READER_SPACE ' '
#define TEXT_FILE_READER_TAB '\t'
#define TEXT_FILE_READER_NEWLINE '\n'

// IMPLEMENTS
// __FREE_REQUIRED for TextKeyValueFileContents.bytes, TextKeyValueFileContents.key_indices, TextKeyValueFileContents.value_indices
// Assumes that r_file_contents.bytes and r_file_contents.num_bytes are already set
// NOTE: This alters r_file_contents.bytes, changing some whitespace to null bytes. This method does not allocate memory for
// TextKeyValueFileContents.bytes nor does it free the existing TextKeyValueFileContents.bytes. Presumably TextKeyValueFileContents.bytes
// was malloc'd previously and needed to be freed at sometime in the future, and that's still the case after this method is called
int convert_file_to_key_values(struct TextKeyValueFileContents* r_file_contents) {
    enum State {
        KEY,
        VALUE,
        WHITESPACE_KEY,
        WHITESPACE_VALUE
    };

    enum CharCategory {
        WHITESPACE,
        NON_WHITESPACE
    };

    if (r_file_contents->num_chars == 0) {
        printf("Cannot convert empty file\n");
        return -1;
    }
    // TODO Not needed?
    // switch(r_file_contents->bytes[0]) {
    //     case TEXT_FILE_READER_SPACE:
    //     case TEXT_FILE_READER_TAB:
    //     case TEXT_FILE_READER_NEWLINE:
    //         printf("Cannot convert a file that starts with whitespace\n");
    //         return -1;
    //     default:
    // }

    State state = WHITESPACE_VALUE;
    CharCategory next_char;
    uint16_t num_keys = 0;
    for (uint64_t i = 0; i < r_file_contents->num_chars; ++i) {
        switch(r_file_contents->chars[i]) {
            case TEXT_FILE_READER_SPACE:
            case TEXT_FILE_READER_TAB:
            case TEXT_FILE_READER_NEWLINE:
            case 0:
                next_char = WHITESPACE;
                break;
            default:
                next_char = NON_WHITESPACE;
                break;
        }

        switch (next_char) {
            case WHITESPACE:
                switch (state) {
                    case WHITESPACE_KEY:
                    case WHITESPACE_VALUE:
                        // Ignore extra whitespace after the first whitespace
                        break;
                    case KEY:
                        r_file_contents->chars[i] = 0;
                        state = WHITESPACE_KEY;
                        break;
                    case VALUE:
                        r_file_contents->chars[i] = 0;
                        state = WHITESPACE_VALUE;
                        break;
                }
                break;
            case NON_WHITESPACE:
                switch (state) {
                    case WHITESPACE_KEY:
                        state = VALUE;
                        break;
                    case WHITESPACE_VALUE:
                        num_keys += 1;
                        state = KEY;
                        break;
                    case KEY:
                    case VALUE:
                        break;
                }
                break;
        }
    }

    // printf("Number of keys is %d\n", num_keys);

    r_file_contents->num_keys = num_keys;
    r_file_contents->key_indices = (uint64_t*)malloc(num_keys * sizeof(uint64_t));
    r_file_contents->value_indices = (uint64_t*)malloc(num_keys * sizeof(uint64_t));

    state = WHITESPACE_VALUE;
    uint16_t key_index = -1;
    for (uint64_t i = 0; i < r_file_contents->num_chars; ++i) {
        switch(r_file_contents->chars[i]) {
            case TEXT_FILE_READER_SPACE:
            case TEXT_FILE_READER_TAB:
            case TEXT_FILE_READER_NEWLINE:
            case 0:
                next_char = WHITESPACE;
                break;
            default:
                next_char = NON_WHITESPACE;
                break;
        }

        switch (next_char) {
            case WHITESPACE:
                switch (state) {
                    case WHITESPACE_KEY:
                    case WHITESPACE_VALUE:
                        // Ignore extra whitespace after the first whitespace
                        break;
                    case KEY:
                        state = WHITESPACE_KEY;
                        break;
                    case VALUE:
                        state = WHITESPACE_VALUE;
                        break;
                }
                break;
            case NON_WHITESPACE:
                switch (state) {
                    case WHITESPACE_KEY:
                        // printf("Found non whitespace %c at %llu, change to value state, set value_indices[%hu]\n", r_file_contents->chars[i], i, key_index);
                        state = VALUE;
                        r_file_contents->value_indices[key_index] = i;
                        break;
                    case WHITESPACE_VALUE:
                        state = KEY;
                        key_index += 1;
                        // printf("Found non whitespace %c at %llu, change to key state, set key_indices[%hu]\n", r_file_contents->chars[i], i, key_index);
                        r_file_contents->key_indices[key_index] = i;
                        break;
                    case KEY:
                    case VALUE:
                        break;
                }
                break;
        }
    }
    // printf("at end key_index is %d\n", key_index);

    return 0;
}

char* dict_get_value(char* key, struct TextKeyValueFileContents* dict) {
    for (int i = 0; i < dict->num_keys; ++i) {
        if (strcmp(key, dict->chars + dict->key_indices[i]) == 0) {
            return dict->chars + dict->value_indices[i];
        }
    }
    printf("Key not found in dict: %s\n", key);
    return NULL;
}