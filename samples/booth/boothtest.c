#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "booth.h"

/*
 * Call regular 8x8 multiply with a function call overhead
 * (for comparison with booth multiply)
 */
int16_t __fastcall__ regular8x8(int8_t x, int8_t y) {
    return ((int16_t)x)*y; /* One argument must be promoted for wide result */
}

/*
 * Call regular 16x16 multiply with a function call overhead
 * (for comparison with booth multiply)
 */

int32_t __fastcall__ regular16x16(int16_t x, int16_t y) {
    return ((int32_t)x)*y; /* One argument must be promoted for wide result */
}

void do_regular8x8() {
    int16_t x,y,z;
    for(x=INT8_MIN;x<=INT8_MAX;++x) {
        for(y=INT8_MIN;y<=INT8_MAX;++y) {
            z=regular8x8((int8_t)x,(int8_t)y);
        }
    }   
}

void do_regular16x16() {
    int32_t x,y,z;
    for(x=-500;x<=500;++x) {
        for(y=-500;y<=500;++y) {
            z=regular16x16((int16_t)x,(int16_t)y);
        }
    }   
}

void do_booth8x8() {
    int16_t x,y,z;
    for(x=INT8_MIN;x<=INT8_MAX;++x) {
        for(y=INT8_MIN;y<=INT8_MAX;++y) {
            z=booth8x8((int8_t)x,(int8_t)y);
        }
    }   
}


void do_booth16x16() {
    int32_t x,y,z;
    for(x=-500;x<=500;++x) {
        for(y=-500;y<=500;++y) {
            z=booth16x16((int16_t)x,(int16_t)y);
        }
    }   
}


/* 
 * Regression test: compare booth 8x8 to internal multiply routines 
 */
 
void compare_booth_regular_8x8() {
    int16_t x,y, boothProduct, product;
    for(x=INT8_MIN;x<=INT8_MAX;++x) {
        for(y=INT8_MIN;y<=INT8_MAX;++y) {            
            boothProduct = booth8x8((int8_t)x, (int8_t)y);
            product = regular8x8((int8_t)x, (int8_t)y);
            assert(boothProduct==product);
        }
    }
}

/* 
 * Regression test: compare booth 16x16 to internal multiply routines 
 */
 
void compare_booth_regular_16x16() {
    int32_t x,y, boothProduct, product;
    for(x=-500;x<=500;++x) {
        for(y=-500;y<=500;++y) {            
            boothProduct = booth16x16((int16_t)x, (int16_t)y);
            product = regular16x16((int16_t)x, (int16_t)y);
            /*assert(boothProduct==product);*/
            if(boothProduct != product) {
                printf("%d x %d expected %08lx, got %08lx\n", (int16_t)x, (int16_t)y, (int32_t)product, (int32_t)boothProduct);
                assert(0);
            }
        }
    }
}

void usage() {
    puts("Usage: boothtest compare[8|16]|regular[8|16]|booth[8|16]\n"); 
}

/*
 * Simple test/benchmark framework
 */
int main(int argc, char**argv) {
    if (argc != 2) {
        usage();
        return 1;
    }
    else {
        char *command = argv[1];
        if       (0==strcmp(command, "compare8")) {
            compare_booth_regular_8x8();
        } else if(0==strcmp(command, "compare16")) {
            compare_booth_regular_16x16();  
        } else if(0==strcmp(command, "regular8")) {
            do_regular8x8();
        } else if(0==strcmp(command, "regular16")) {
            do_regular16x16();
        } else if(0==strcmp(command, "booth8")) {
            do_booth8x8();
        } else if(0==strcmp(command, "booth16")) {
            do_booth16x16();
        } else {
            usage();
            return 1;
        }
    }
    return 0;
}

