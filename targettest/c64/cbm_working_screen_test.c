
/* this program prints a text onto the standard screen (e.g. for C64 at $0400) and then switches the
   working screen to another location and prints a string there. Then it checks, if both strings are found
   at the correct memory location */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "../../include/cbm.h"


char petscii2scrcode(char ch);


int test_boundaries(void)
{
    struct testdata {
        unsigned char screen_hi;
        int returncode;
    };

    struct testdata testdata_input[] = {
        0x00, EXIT_FAILURE,
        0x04, EXIT_SUCCESS,
        0x0c, EXIT_SUCCESS,
        0x10, EXIT_FAILURE, // Cannot set screen to $1000-$1c00 due to shadowing
        0x1c, EXIT_FAILURE,
        0x20, EXIT_SUCCESS,
        0x8c, EXIT_SUCCESS,
        0x90, EXIT_FAILURE, // Cannot set screen to $9000-$9c00 due to shadowing
        0x9c, EXIT_FAILURE,
        0xa0, EXIT_SUCCESS,
        0xc0, EXIT_SUCCESS,
        0x02, EXIT_FAILURE, // SCREEN not aligned to $0400 boundary
        0x41, EXIT_FAILURE,
        0x72, EXIT_FAILURE,
        0xc3, EXIT_FAILURE,
    };

    int i, returncode;

    printf("Testing different desired workscreen destinations:\n");
    for (i = sizeof(testdata_input)/sizeof(testdata_input[0]) - 1; i>=0; --i){
        printf("Screen @$%04x: ", testdata_input[i].screen_hi*0x100);
        returncode = cbm_set_working_screen(testdata_input[i].screen_hi);
        cbm_reset_working_screen();

        if (returncode == EXIT_SUCCESS){
            printf("OK\n");
        } else {
            printf("DENY\n");
        }

        if (returncode != testdata_input[i].returncode){
            printf(
                "Boundary check failed on screen hi %hhx!\n"
                "Function cbm_set_working_screen returned %d instead of %d\n",
                testdata_input[i].screen_hi,
                returncode, testdata_input[i].returncode);

                return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}


int str_to_scrcode(char *str){
    char* ch_ptr = str;

    for (;*ch_ptr != '\0'; ++ch_ptr){
        *ch_ptr = petscii2scrcode(*ch_ptr);
    }

    return 0;
}


int test_str_found_at(char *test_str, uint8_t scr_loc){
    printf("Test printing to working screen at %hhx:\n", scr_loc);

    cbm_set_working_screen(scr_loc);

    putchar(CH_HOME + 0x80);
    puts(test_str);
    str_to_scrcode(test_str);
    cbm_reset_working_screen();
    if (strnicmp(test_str, (char*)(scr_loc * 0x100), strlen(test_str)) != 0){
        printf("Failed to print to screen location at %04x", scr_loc * 0x100);
        return -1;
    }   

    return 0;
}


int main(void)
{
    char *test_string1 = "foo";
    char *test_string2 = "bar";
    size_t testcase_count = 3;
    size_t i;
    int returncode, failed_tests = 0;

    for (i = testcase_count; i>0; --i){
        switch(i){
        case 3:
            returncode = test_boundaries();
            break;
        case 2:
            returncode = test_str_found_at(test_string1, 0xc0);
            break;
        case 1:
            returncode = test_str_found_at(test_string2, 0xc4);
            break;
        default:
            assert(0);  // Should never come here
        }
        if (returncode == EXIT_FAILURE){
            ++failed_tests;
        }
    }

    return failed_tests;
}
