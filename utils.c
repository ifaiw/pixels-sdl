#ifndef _UTILS__C
#define _UTILS__C

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>

#include "game_structs.h"

#define LITTLE_ENDIAN_32(array_name, offset) array_name[offset] << 24 | array_name[offset + 1] << 16 | array_name[offset + 2] << 8 | array_name[offset + 3]
#define BIG_ENDIAN_32(array_name, offset) array_name[offset] | array_name[offset + 1] << 8 | array_name[offset + 2] << 16 | array_name[offset + 3] << 24
#define LITTLE_ENDIAN_16(array_name, offset) array_name[offset] << 8 | array_name[offset + 1]
#define BIG_ENDIAN_16(array_name, offset) array_name[offset] | array_name[offset + 1] << 8


enum Endianess {
    BIG_ENDIAN_ENDIANESS,
    LITTLE_ENDIAN_ENDIANESS
};
inline uint32_t get_uint32(uint8_t* array, int offset, enum Endianess endianess) {
    if (endianess == BIG_ENDIAN_ENDIANESS) {
        return BIG_ENDIAN_32(array, offset);
    }
    return LITTLE_ENDIAN_32(array, offset);
}
inline uint32_t get_uint16(uint8_t* array, int offset, enum Endianess endianess) {
    if (endianess == BIG_ENDIAN_ENDIANESS) {
        return BIG_ENDIAN_16(array, offset);
    }
    return LITTLE_ENDIAN_16(array, offset);
}

// void to_byte_str(uint8_t b, int buffer_index, char* o_buffer) {
//     printf("b is %d\n", b);
//     uint8_t lower_hex_val = b & 0x0F;
//     uint8_t upper_hex_val = (b & 0xF0) >> 4;
//     // printf("low=%d hi=%d\n", lower_hex_val, upper_hex_val);
//     assert(lower_hex_val < 16);
//     assert(upper_hex_val < 16);
//     if (lower_hex_val <= 9) {
//         o_buffer[buffer_index + 1] = '0' + lower_hex_val;
//     } else {
//         o_buffer[buffer_index + 1] = 'A' - 10 + lower_hex_val;
//     }
//     if (upper_hex_val <= 9) {
//         o_buffer[buffer_index] = '0' + upper_hex_val;
//     } else {
//         o_buffer[buffer_index] = 'A' - 10 + upper_hex_val;
//     }

//     // for (int i = 15; i >= 0; --i) {
//     //     uint8_t next_byte = b & 0xf;
//     //     assert(next_byte < 16);
//     //     if (next_byte < 9) {
//     //         o_buffer[buffer_index + i] = '0' + next_byte;
//     //     } else {
//     //         o_buffer[buffer_index + i] = 'A' - 10 + next_byte;
//     //     }
//     //     // printf("next_byte is %" PRIu8 "\n", next_byte);
//     //     b = b >> 4;
//     // }
// }

// void to_byte_str32(uint32_t b, int buffer_index, char* o_buffer) {
//     for (int i = 47; i >= 0; --i) {
//         uint32_t next_byte = b & 0xf;
//         assert(next_byte < 16);
//         if (next_byte < 9) {
//             o_buffer[buffer_index + i] = '0' + next_byte;
//         } else {
//             o_buffer[buffer_index + i] = 'A' - 10 + next_byte;
//         }
//         b = b >> 4;
//     }
// }

inline long diff_time_in_nanos(struct timespec *t1, struct timespec *t2) {
    long diff_in_nanos_from_sec = (t2->tv_sec - t1->tv_sec) * (long)1000000000;
    long diff_in_nanos_from_nanos = t2->tv_nsec - t1->tv_nsec;
    return diff_in_nanos_from_sec + diff_in_nanos_from_nanos;
}

inline long diff_time_in_micros(struct timespec *t1, long t2_secs, long t2_nanos) {
    long diff_in_micros_from_sec = (t2_secs - t1->tv_sec) * (long)1000000;
    long diff_in_micros_from_nanos = (t2_nanos - t1->tv_nsec) / (long)1000;
    return diff_in_micros_from_sec + diff_in_micros_from_nanos;
}

inline long micros_until_next_frame(struct timespec *initial, struct timespec *current, long next_frame_count, long nanos_per_frame) {
    long nanos_of_next_frame_since_initial = next_frame_count * nanos_per_frame;
    long seconds_of_next_frame_since_initial = nanos_of_next_frame_since_initial / (long)1000000000;
    long nanos_remainder = nanos_of_next_frame_since_initial - (seconds_of_next_frame_since_initial * (long)1000000000);

    long seconds_next_frame = initial->tv_sec + seconds_of_next_frame_since_initial;
    long nanos_next_frame = initial->tv_nsec + nanos_remainder;

    return diff_time_in_micros(current, seconds_next_frame, nanos_next_frame);
}

// TODO  does static on these functions actually do anything useful?
inline int get_min(int a, int b) {
    int c = a < b ? a : b;
    if (a < b) {
        return a;
    }
    return b;
}

#endif // _UTILS__C