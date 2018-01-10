#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <cc65.h>

#include "booth.h"

void do_regular8x8() {
    int16_t x,y,z;
    for(x=INT8_MIN;x<=INT8_MAX;++x) {
        for(y=INT8_MIN;y<=INT8_MAX;++y) {
            z=cc65_imul8x8r16((int8_t)x,(int8_t)y);
        }
    }   
}

void do_regular16x16() {
    int16_t x,y;
    int32_t z;
    for(x=-500;x<=500;++x) {
        for(y=-500;y<=500;++y) {
            z=cc65_imul16x16r32((int16_t)x,(int16_t)y);
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
    int16_t x,y;
    int32_t z;
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
            product = cc65_imul8x8r16((int8_t)x, (int8_t)y);
            assert(boothProduct==product);
        }
    }
}

/* 
 * Regression test: compare booth 16x16 to internal multiply routines 
 */
 
void compare_booth_regular_16x16() {
    int16_t x,y;
    int32_t boothProduct, product;
    for(x=-500;x<=500;++x) {
        for(y=-500;y<=500;++y) {            
            boothProduct = booth16x16((int16_t)x, (int16_t)y);
            product = cc65_imul16x16r32((int16_t)x, (int16_t)y);
            assert(boothProduct==product);
        }
    }
}

/*
 * Print usage
 */

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

