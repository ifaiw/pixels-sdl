#ifndef _UTILS__C
#define _UTILS__C

#include <inttypes.h>
#include <time.h>

static void to_byte_str(uint8_t b, int buffer_index, char* o_buffer) {
    for (int i = 15; i >= 0; --i) {
        uint8_t next_byte = b & 0xf;
        assert(next_byte < 16);
        if (next_byte < 9) {
            o_buffer[buffer_index + i] = '0' + next_byte;
        } else {
            o_buffer[buffer_index + i] = 'A' - 10 + next_byte;
        }
        // printf("next_byte is %" PRIu8 "\n", next_byte);
        b = b >> 4;
    }
}

static void to_byte_str32(uint32_t b, int buffer_index, char* o_buffer) {
    for (int i = 47; i >= 0; --i) {
        uint32_t next_byte = b & 0xf;
        assert(next_byte < 16);
        if (next_byte < 9) {
            o_buffer[buffer_index + i] = '0' + next_byte;
        } else {
            o_buffer[buffer_index + i] = 'A' - 10 + next_byte;
        }
        b = b >> 4;
    }
}

static inline long diff_time_in_nanos(struct timespec *t1, struct timespec *t2) {
    long diff_in_nanos_from_sec = (t2->tv_sec - t1->tv_sec) * (long)1000000000;
    long diff_in_nanos_from_nanos = t2->tv_nsec - t1->tv_nsec;
    return diff_in_nanos_from_sec + diff_in_nanos_from_nanos;
}

static inline long diff_time_in_micros(struct timespec *t1, long t2_secs, long t2_nanos) {
    long diff_in_micros_from_sec = (t2_secs - t1->tv_sec) * (long)1000000;
    long diff_in_micros_from_nanos = (t2_nanos - t1->tv_nsec) / (long)1000;
    return diff_in_micros_from_sec + diff_in_micros_from_nanos;
}

static inline long micros_until_next_frame(struct timespec *initial, struct timespec *current, long next_frame_count, long nanos_per_frame) {
    long nanos_of_next_frame_since_initial = next_frame_count * nanos_per_frame;
    long seconds_of_next_frame_since_initial = nanos_of_next_frame_since_initial / (long)1000000000;
    long nanos_remainder = nanos_of_next_frame_since_initial - (seconds_of_next_frame_since_initial * (long)1000000000);

    long seconds_next_frame = initial->tv_sec + seconds_of_next_frame_since_initial;
    long nanos_next_frame = initial->tv_nsec + nanos_remainder;

    return diff_time_in_micros(current, seconds_next_frame, nanos_next_frame);
}

#endif // _UTILS__C