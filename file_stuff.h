#ifndef _FILE_STUFF__H
#define _FILE_STUFF__H

#include <inttypes.h>

// __FREE_REQUIRED for FileBytes.bytes
struct FileBytes {
    uint8_t* bytes;
    uint64_t num_bytes;
};

// __FREE_REQUIRED for TextKeyValueFileContents.bytes, TextKeyValueFileContents.key_indices, TextKeyValueFileContents.value_indices
struct TextKeyValueFileContents {
    char* chars;
    uint64_t num_chars;
    uint16_t num_keys;
    uint64_t* key_indices;
    uint64_t* value_indices;
};

// __FREE_REQUIRED for r_file_bytes.bytes
int read_file(const char* file_path, struct FileBytes* r_file_bytes);

// fail_if_exists == "true" if not 0
int write_file(const char* file_path, struct FileBytes* file_bytes, uint8_t fail_if_exists);

// __FREE_REQUIRED for r_file_contents.chars, r_file_contents.key_indices, r_file_contents.value_indices
// Assumes that r_file_contents.chars and r_file_contents.num_bytes are already set
// NOTE: This alters r_file_contents.chars, changing some whitespace to null bytes. This method does not allocate memory for
// TextKeyValueFileContents.chars nor does it free the existing TextKeyValueFileContents.chars. Presumably TextKeyValueFileContents.chars
// was malloc'd previously and needed to be freed at sometime in the future, and that's still the case after this method is called
int convert_file_to_key_values(struct TextKeyValueFileContents* r_file_contents);

char* dict_get_value(char* key, struct TextKeyValueFileContents* dict);



#endif  // _FILE_STUFF__H