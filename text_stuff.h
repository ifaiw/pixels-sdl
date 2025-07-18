#ifndef _TEXT_STUFF__H_ONLY
#define _TEXT_STUFF__H_ONLY

// TODO stdio is just for printf
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// __FREE_REQUIRED for returned value
// Find first instance of `{ANY_TEXT}` placeholder in main. If no substrings to replace then NULL returned.
// NOTE: main and to_replace_with must be null-terminated
char* replace_once(const char* main, const char* to_replace_with) {
    size_t left = 0;
    size_t right;
    size_t last;
    size_t len_to_replace_with = 0;
    while (to_replace_with[len_to_replace_with] != 0) {
        ++len_to_replace_with;
    }

    while (main[left] != 0 && main[left] != '{') {
        ++left;
    }
    if (main[left] == 0) {
        // No placeholder found
        return NULL;
    }

    right = left + 1;
    while (main[right] != '}') {
        ++right;
    }

    last = right + 1;
    while (main[last] != 0) {
        ++last;
    }

    size_t len_new_string = left + len_to_replace_with + (last - right - 1);
    printf("len_new_string %zu\n", len_new_string);
    printf("left %zu\n", left);
    char* result = (char*)malloc(len_new_string);
    memcpy(result, main, left);
    memcpy(result + left, to_replace_with, len_to_replace_with);
    memcpy(result + left + len_to_replace_with, main + right + 1, last - right - 1);

    return result;
}

#endif  // _TEXT_STUFF__H_ONLY