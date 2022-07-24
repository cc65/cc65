/* bug #327 - Promoting u8 to s16 gives wrong result */

#include <stdio.h>
#include <stdint.h>

static const uint8_t arr[2] = {
    0,
    255
};

static int16_t get16() {
    return -arr[1];
}

static int16_t get16_2() {
    return -(int16_t) arr[1];
}

char res = 0;

int main() {

    printf("Value %d, should be -255\n", get16());
    printf("Value %d, should be -255\n", get16_2());

    if (get16() != -255) {
        res++;
    }
    if (get16_2() != -255) {
        res++;
    }

    return res;
}
