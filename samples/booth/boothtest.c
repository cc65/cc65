#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "booth.h"

/*
 * Call regular multiply with a function call overhead
 * (for comparison with booth multiply)
 */
int16_t __fastcall__ regular8x8(int8_t x, int8_t y) {
	return x*y;
}

void do_regular8x8() {
	int16_t x,y,z;
	for(x=-127;x<=128;++x) {
		for(y=-127;y<=128;++y) {
			z=regular8x8((int8_t)x,(int8_t)y);
		}
	}	
}

void do_booth8x8() {
	int16_t x,y,z;
	for(x=-127;x<=128;++x) {
		for(y=-127;y<=128;++y) {
			z=booth8x8((int8_t)x,(int8_t)y);
		}
	}	
}

/* 
 * Regression test: compare booth to internal multiply routines 
 */
 
void compare_booth_regular_8x8() {
	int16_t x,y;
	int16_t boothProduct;
	int16_t product;
	for(x=-127;x<=128;++x) {
		for(y=-127;y<=128;++y) {			
			boothProduct = booth8x8((int8_t)x, (int8_t)y);
			product = regular8x8((int8_t)x, (int8_t)y);
			assert(boothProduct==product);
		}
	}
}

void usage() {
	puts("Usage: boothtest compare|regular|booth\n"); 
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
		if(0==strcmp(command, "compare")) {
			compare_booth_regular_8x8();
		} else if(0==strcmp(command, "regular")) {
			do_regular8x8();
		} else if(0==strcmp(command, "booth")) {
			do_booth8x8();
		} else {
			usage();
			return 1;
		}
	}
	return 0;
}

