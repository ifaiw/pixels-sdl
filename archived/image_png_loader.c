#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>

// included just to get memcpy
#include <string.h>

#include "utils.c"

#include "image_png_loader.h"


#define MIN_FILE_SIZE_IN_BYTES 20

const uint8_t PNG_HEADER[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
const int PNG_HEADER_LENGTH = 8;


// zlib buffer chunk size. 256kb
const int CHUNK = 262144;


inline int get_min(int a, int b) {
    int c = a < b ? a : b;
    if (a < b) {
        return a;
    }
    return b;
}

// __FREE_REQUIRED for r_buffer_decompressed AND r_buffer_decompressed_size
int decompress(uint8_t* compressed_bytes, int compressed_length, int decompressed_size_guess, uint8_t** r_buffer_decompressed, int** r_buffer_decompressed_size) {
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    int temp_decompress_size = decompressed_size_guess;
    if (temp_decompress_size <= CHUNK) {
        printf("WARNING: decompressed_size_guess is not bigger than CHUNK: %d, %d\n", temp_decompress_size, CHUNK);
        temp_decompress_size = CHUNK * 2;
    }

    unsigned char* temp_decompress_buffer = (unsigned char*)malloc(temp_decompress_size);

    int result = inflateInit(&strm);
    if (result != Z_OK) {
        printf("inflateInit failed with code %d\n", result);
        return result;
    }

    unsigned have;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
    int in_index = 0;
    int out_index = 0;

    for (;;) {
        printf("at top in_index is %d\n", in_index);

        // int amount_read = in_index + CHUNK < compressed_length ? CHUNK : compressed_length - in_index;
        // memcpy(in, compressed_bytes + in_index, amount_read);
        // in_index += amount_read;
        // strm.avail_in = amount_read;
        // strm.next_in = in;
        if (in_index >= compressed_length) {
            break;
        }

        int amount_read = in_index + CHUNK < compressed_length ? CHUNK : compressed_length - in_index;
        if (amount_read == 0) {
            printf("WARNING: amount_read==0, breaking out of outer inflate loop\n");
            break;
        }
        printf("amount_read is %d\n", amount_read);
        // TODO only if using in
        memcpy(in, compressed_bytes + in_index, amount_read);

        strm.avail_in = amount_read;
        strm.next_in = in;
        in_index += amount_read;

        for (;;) {
            printf("top of inner loop\n");
            strm.avail_out = CHUNK;
            strm.next_out = out;

            result = inflate(&strm, Z_NO_FLUSH);
            printf("inflate result is %d\n", result);
            assert(result != Z_STREAM_ERROR);  /* state not clobbered */
            switch (result) {
                case Z_NEED_DICT:
                    printf("WARNING: Z_NEED_DICT, is data error\n");
                    result = Z_DATA_ERROR;     /* and fall through */
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    (void)inflateEnd(&strm);
                    return result;
            }

            have = CHUNK - strm.avail_out;
            printf("inflating, have=%u\n", have);
            printf("the %u bytes of out\n", have);
            for (int i = 0; i < have; ++i) {
                printf("%02x", out[i]);
            }
            printf("\n");

            if (out_index + have > temp_decompress_size) {
                printf("Need more temp_decompress_size\n");
                int new_temp_decompress_size = temp_decompress_size * 2;
                printf("Need more temp_decompress_size, increasing to %d\n", new_temp_decompress_size);
                unsigned char* new_temp_decompress_buffer = (unsigned char*)malloc(new_temp_decompress_size);
                memcpy(new_temp_decompress_buffer, temp_decompress_buffer, out_index);
                free(temp_decompress_buffer);
                temp_decompress_buffer = new_temp_decompress_buffer;
                temp_decompress_size = new_temp_decompress_size;
            }
            memcpy(temp_decompress_buffer + out_index, out, have);
            out_index += have;

            printf("at bottom of inner loop out_index=%d strm.avail_out=%d\n", out_index, strm.avail_out);
            if (strm.avail_out != 0) {
                 break;
            }
        }
        printf("at bottom of outer loop strm.avail_in=%d\n", in_index);

        assert(strm.avail_in == 0);
    }

    printf("Past all decompressing, copy to r_buffer_decompressed\n");

    *r_buffer_decompressed_size = (int *)malloc(sizeof(int));
    **r_buffer_decompressed_size = out_index;
    *r_buffer_decompressed = (uint8_t*)malloc(out_index);
    memcpy(*r_buffer_decompressed, temp_decompress_buffer, out_index);

    free(temp_decompress_buffer);
    inflateEnd(&strm);

    return 0;
}

// __FREE_REQUIRED for r_image_info.pixels
int load_image(const char* file_path, struct Image_info* r_image_info) {

    FILE* file_ptr;

    file_ptr = fopen(file_path, "rb");
    if (file_ptr == NULL) {
       printf("Error opening file\n");
       return 1;
    }

    long length;
    fseek(file_ptr, 0, SEEK_END);
    length = ftell(file_ptr);
    rewind(file_ptr);

    printf("file length is %ld\n", length);

    if (length < MIN_FILE_SIZE_IN_BYTES) {
        printf("File is too small to be a PNG: %ld", length);
        return 1;
    }

    uint8_t* buffer = (uint8_t*)malloc(length * sizeof(uint8_t));
    long bytes_read = length * fread(buffer, length, 1, file_ptr);
    if (bytes_read != length) {
       printf("bytes_read differs from length: %ld, %ld\n", bytes_read, length);
       return 1;
    }
    fclose(file_ptr);

    char l[17];
    // for (int i = 0; i < 8; ++i) {
    //     l[i] = '-';
    // }
    l[16] = '\0';
    // printf("about to to_byte_str *%s*\n", l);
    for (int i = 0; i < 8; ++i) {
        to_byte_str(buffer[i], 2*i, l);
    }

    printf("done to_byte_str\n");
    printf("First 8 bytes are *%s*\n", l);

    // int64_t expected_header = (int64_t)PNG_HEADER;
    // int64_t header = ((int64_t*)buffer)[0];
    // if (header != expected_header) {
    //     printf("Header is not the PNG header. Expected %lld found %lld\n", expected_header, header);
    //     return 1;
    // }
    for (int i = 0; i < PNG_HEADER_LENGTH; ++i) {
        if (buffer[i] != PNG_HEADER[i]) {
            printf("Header is not the PNG header. At byte %d expected %d found %d\n", i, PNG_HEADER[i], buffer[i]);
            return 1;
        }
    }

    // First chunk starts after the PNG header
    // I'm assuming/hoping it's a simple PNG, in which case I ignore the first chunk which should be short and just
    // describing the image format/compression etc, and just load the second chunk directly as pixels
    // TODO NEED TO GET THE HEIGHT AND WIDTH FROM HERE
    int chunk_offset = PNG_HEADER_LENGTH;
    uint32_t chunk_length = buffer[chunk_offset] << 24 | buffer[chunk_offset + 1] << 16 | buffer[chunk_offset + 2] << 8 | buffer[chunk_offset + 3];
    printf("chunk1 length_field is %d\n", chunk_length);
    // 4 bytes for chunk length, 4 bytes for chunk type, the chunk, 4 bytes for crc
    chunk_offset += 4 + 4 + chunk_length + 4;
    if (chunk_offset > length) {
        printf("Calculated end of chunk1 is past end of file. end=%d length=%ld\n", chunk_offset, length);
        return 1;
    }

    // TODO just for debugging
    // For Decors.png header:
    // width 224
    // height 112
    // bit depth 8
    // color type 6
    // compression method 0
    // filler method 0
    // interlace method 0


    // Second chunk is pixels?
    chunk_length = buffer[chunk_offset] << 24 | buffer[chunk_offset + 1] << 16 | buffer[chunk_offset + 2] << 8 | buffer[chunk_offset + 3];
    printf("chunk2 length_field is %d\n", chunk_length);
    // 4 bytes for chunk length, 4 bytes for chunk type, the chunk, 4 bytes for crc
    int pixels_offset = chunk_offset + 8;
    chunk_offset += 4 + 4 + chunk_length + 4;
    if (chunk_offset > length) {
        printf("Calculated end of chunk2 is past end of file. end=%d length=%ld\n", chunk_offset, length);
        return 1;
    }

    // TODO use actual height and width. 224 * 112 * 4 should be the number of pixel bytes, then need some some of extra buffer to be safe
    uint8_t* pixel_bytes = NULL;
    int* pixel_bytes_size = NULL;
    // __FREE_REQUIRED for pixel_bytes and pixel_bytes_size
    int decompress_result = decompress(buffer + pixels_offset, chunk_length, 224 * 112 * 4 * 2, &pixel_bytes, &pixel_bytes_size);
    printf("decompress_result is %d\n", decompress_result);
    if (decompress_result != 0) {
        printf("decompress failed! %d\n", decompress_result);
    }
    printf("null check pixel_bytes: %d\n", pixel_bytes == NULL);
    printf("null check pixel_bytes_size: %d\n", pixel_bytes_size == NULL);
    printf("pixel_bytes_size: %d\n", *pixel_bytes_size);
    printf("About to assign r_image_info->pixels\n");
    r_image_info->pixels = (uint32_t*)pixel_bytes;
    printf("About to assign r_image_info->pixels\n");
    r_image_info->height = 112;
    r_image_info->width = 224;
    printf("about to free pixel_bytes_size\n");
    free(pixel_bytes_size);
    printf("Done free pixel_bytes_size\n");

    // There should be at least one more chunk, the IEND chunk marking the end of the PNG data
    chunk_length = buffer[chunk_offset] << 24 | buffer[chunk_offset + 1] << 16 | buffer[chunk_offset + 2] << 8 | buffer[chunk_offset + 3];
    printf("chunk3 length_field is %d\n", chunk_length);
    // 4 bytes for chunk length, 4 bytes for chunk type, the chunk, 4 bytes for crc
    chunk_offset += 4 + 4 + chunk_length + 4;
    if (chunk_offset > length) {
        printf("Calculated end of chunk3 is past end of file. end=%d length=%ld\n", chunk_offset, length);
        return 1;
    }
    if (chunk_offset < length) {
        printf("Calculated end of chunk3 is before the end of the file, so maybe there's more chunks?. end=%d length=%ld\n", chunk_offset, length);
        return 1;
    }

    printf("about to free buffer\n");
    free(buffer);
    return 0;
}