#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// included just to get memcpy
#include <string.h>

#include "utils.c"

#include "image_bmp_loader.h"


#define MIN_FILE_SIZE_IN_BYTES 10


const uint8_t BITMAPFILEHEADER[] = {0x42, 0x4D};


inline void print_file_header(uint32_t file_size_from_header, uint32_t bitmap_data_start) {
    printf("BitmapFileHeader:\n  file_size_from_header=%u\n  bitmap_data_start=%u\n\n", file_size_from_header, bitmap_data_start);
}

inline void print_info_header(  uint32_t info_header_size,
                                uint32_t width,
                                uint32_t height,
                                uint16_t number_of_planes,
                                uint16_t bits_per_pixel,
                                uint32_t num_colors,
                                uint32_t compression_type,
                                uint32_t pixels_per_meter_horizontal,
                                uint32_t pixels_per_meter_vertical,
                                uint32_t num_colors_actually_used,
                                uint32_t num_important_colors,
                                uint32_t expected_color_table_size_in_bytes
)
{
    printf("BitmapInfoHeader:\n");
    printf("  info_header_size=%u\n", info_header_size);
    printf("  width=%u\n", width);
    printf("  height=%u\n", height);
    printf("  number_of_planes=%u\n", number_of_planes);
    printf("  bits_per_pixel=%u\n", bits_per_pixel);
    printf("  num_colors=%u\n", num_colors);
    printf("  compression_type=%u\n", compression_type);
    printf("  pixels_per_meter_horizontal=%u\n", pixels_per_meter_horizontal);
    printf("  pixels_per_meter_vertical=%u\n", pixels_per_meter_vertical);
    printf("  num_colors_actually_used=%u\n", num_colors_actually_used);
    printf("  num_important_colors=%u\n", num_important_colors);
    printf("  expected_color_table_size_in_bytes=%u\n", expected_color_table_size_in_bytes);
}


// __FREE_REQUIRED for r_image_info.pixels
int load_bmp_image(const char* file_path, struct ImageInfo* r_image_info) {

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
        printf("File is too small to be a BMP: %ld", length);
        return 1;
    }

    uint8_t* buffer = (uint8_t*)malloc(length * sizeof(uint8_t));
    long bytes_read = length * fread(buffer, length, 1, file_ptr);
    if (bytes_read != length) {
       printf("bytes_read differs from length: %ld, %ld\n", bytes_read, length);
       return 1;
    }
    fclose(file_ptr);

    // BMP _file_ header (BITMAPFILEHEADER) is 14 bytes:
    // First two bytes are 'BM'
    // Next 4 bytes are the size of the file in bytes.
    // Next 4 bytes are two "reserved" fields that are always 0.
    // Next 4 bytes are the offset in bytes from the beginning of the file to the beginning of the bitmap data.
    int offset = 0;
    enum Endianess endianess = BIG_ENDIAN_ENDIANESS;
    for (int i = 0; i < 2; ++i) {
        if (buffer[offset + i] != BITMAPFILEHEADER[i]) {
            printf("buffer[0]=%u BITMAPFILEHEADER[0]=%u\n", buffer[0], BITMAPFILEHEADER[0]);
            printf("BITMAPFILEHEADER mismatch at %d\n", i);
            return -1;
        }
    }
    offset += 2;
    uint32_t file_size_from_header = get_uint32(buffer, offset, endianess);
    if (file_size_from_header != length) {
        printf("BMP file size in header doesn't match actual file size: %u, %ld\n", file_size_from_header, length);
        return -2;
    }
    offset += 4;
    for (int i = 0; i < 4; ++i) {
        if (buffer[offset + i] != 0) {
            printf("BITMAPFILEHEADER reserved at %d is not 0: %d\n", (offset + i), buffer[offset + i]);
            return -2;
        }
    }
    offset += 4;
    uint32_t bitmap_data_start = get_uint32(buffer, offset, endianess);
    offset += 4;
    printf("end of bmp file header: %d bitmap_data_start=%u\n", offset, bitmap_data_start);


    // BMP _info_ header (BITMAPINFOHEADER) is 40 bytes:
    // First four bytes are the size of the info header (multiple sources say this, but in my test bmp the value of this field is larger than 40)
    // Also, the description of the color table below says it's variable size, which means this whole header has to be variable
    // Next 4 bytes are the width of the image in pixels
    // Next 4 bytes are the height of the image in pixels
    // Next 2 bytes are the number of planes, should always be 1
    // Next 2 bytes are the number of bits per pixel
    //    1 = monochrome palette. NumColors = 1
    //    4 = 4bit palletized. NumColors = 16
    //    8 = 8bit palletized. NumColors = 256
    //    16 = 16bit RGB. NumColors = 65536 (?)
    //    24 = 24bit RGB. NumColors = 16M
    // Next 4 bytes are the type of compresssion, 0 means uncompressed
    // Next 4 bytes are the horizontal pixels per meter
    // Next 4 bytes are the vertical pixels per meter
    // Next 4 bytes are the number of colors actually used
    // Next 4 bytes are the number of "important" colors, 0 is all colors are important
    // Next 4 * NumColors, where NumColors comes from bits per pixel above, is ColorTable. ColorTable only present if bitsPerPixel <= 8
    //      Each 4 bytes is 1 byte each for RGB in that order, then last byte is 0/unused
    uint32_t info_header_size = get_uint32(buffer, offset, endianess);
    offset += 4;
    uint32_t width = get_uint32(buffer, offset, endianess);
    offset += 4;
    uint32_t height = get_uint32(buffer, offset, endianess);
    offset += 4;
    uint16_t number_of_planes = get_uint16(buffer, offset, endianess);
    offset += 2;
    uint16_t bits_per_pixel = get_uint16(buffer, offset, endianess);
    offset += 2;
    if (bits_per_pixel != 32) {
        printf("WARNING: bits_per_pixel is not 32: %u\n", bits_per_pixel);
    }
    uint32_t num_colors;
    switch (bits_per_pixel) {
        case 1: num_colors = 1; break;
        case 4: num_colors = 16; break;
        case 8: num_colors = 256; break;
        case 16: num_colors = 65536; break;
        case 24: num_colors = 16000000; break;
        // 32 should be still just 24 for the colors and then 8 for alpha that just be unused padding
        // Or maybe the alpha is used to distinguish between black pixels and unused/transparent pixels
        case 32: num_colors = 16000000; break;
        default: num_colors = -1;
    }
    if (num_colors == -1) {
        printf("WARNING: Invalid bits per pixel: %u\n", bits_per_pixel);
    }
    else {
        printf("bits_per_pixel=%u num_colors=%u\n", bits_per_pixel, num_colors);
    }
    uint32_t compression_type = get_uint32(buffer, offset, endianess);
    offset += 4;
    if (compression_type != 0) {
        printf("WARNING: Compression type is non-zero: %u\n", compression_type);
    }
    uint32_t pixels_per_meter_horizontal = get_uint32(buffer, offset, endianess);
    offset += 4;
    uint32_t pixels_per_meter_vertical = get_uint32(buffer, offset, endianess);
    offset += 4;
    uint32_t num_colors_actually_used = get_uint32(buffer, offset, endianess);
    offset += 4;
    uint32_t num_important_colors = get_uint32(buffer, offset, endianess);
    offset += 4;
    uint32_t expected_color_table_size_in_bytes;
    if (num_colors >= 1 && num_colors <= 256) {
        expected_color_table_size_in_bytes = 4 * num_colors;
    } else {
        expected_color_table_size_in_bytes = 0;
    }
    if (expected_color_table_size_in_bytes > 0) {
        for (int i = 0; i < num_colors; ++i) {
            uint32_t color_in_color_table = get_uint32(buffer, offset, endianess);
            offset += 4;
            printf("color in table is %x\n", color_in_color_table);
        }
    }

    print_file_header(file_size_from_header, bitmap_data_start);
    print_info_header(  info_header_size,
                        width,
                        height,
                        number_of_planes,
                        bits_per_pixel,
                        num_colors,
                        compression_type,
                        pixels_per_meter_horizontal,
                        pixels_per_meter_vertical,
                        num_colors_actually_used,
                        num_important_colors,
                        expected_color_table_size_in_bytes
    );

    long bytes_in_file_after_bitmap_data_start = length - bitmap_data_start;
    uint32_t calculated_pixel_bytes = bits_per_pixel / 8 * width * height;
    printf("bytes in file after bitmap_data_start: %ld. Calculated pixel bytes: %u\n", bytes_in_file_after_bitmap_data_start, calculated_pixel_bytes);
    if (bytes_in_file_after_bitmap_data_start != calculated_pixel_bytes) {
        printf("WARNING: bytes_in_file_after_bitmap_data_start != calculated_pixel_bytes\n");
    }

    printf("Current offset is %d\n", offset);
    // while (offset < bitmap_data_start) {
    //     printf("byte between %d is %u\n", offset, buffer[offset]);
    //     offset += 1;
    // }
    offset = bitmap_data_start;

    r_image_info->height = height;
    r_image_info->width = width;
    r_image_info->pixels = (uint32_t*)malloc(bytes_in_file_after_bitmap_data_start);
    memcpy(r_image_info->pixels, buffer + offset, bytes_in_file_after_bitmap_data_start);

    return 0;
}