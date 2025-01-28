#include <inttypes.h>
#include <stdio.h>

uint8_t LEFT_MASK = 0b00000001;
uint8_t RIGHT_MASK = 0b00000010;
uint8_t UP_MASK = 0b00000100;
uint8_t DOWN_MASK = 0b00001000;

int main(int argc, char* argv[]) {
    for (int down_mask = 0; down_mask < 2; down_mask++) {
        for (int up_mask = 0; up_mask < 2; up_mask++) {
            for (int right_mask = 0; right_mask < 2; right_mask++) {
                for (int left_mask = 0; left_mask < 2; left_mask++) {
                    int total_mask = down_mask * DOWN_MASK + up_mask * UP_MASK + right_mask * RIGHT_MASK + left_mask * left_mask;
                    if (left_mask) {
                        printf("LEFT_");
                    }
                    if (right_mask) {
                        printf("RIGHT_");
                    }
                    if (up_mask) {
                        printf("UP_");
                    }
                    if (down_mask) {
                        printf("DOWN_");
                    }
                    printf(" %d\n", total_mask);
                }
            }
        }
    }
}