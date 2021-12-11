/* Accelerator test code. */

#ifndef ACC_DETECT
#error This file cannot be used directly (yet)
#endif

#include <time.h>
#include <accelerator.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

static void print_time_taken(void)
{
    clock_t curtime = clock();
    clock_t newtime;
    unsigned long i;
    char buffer[10];

    printf("Doing a speed test, please wait\n");
    for (i = 0; i < 0x1000; i++) { }
    newtime = clock() - curtime;
    ultoa(newtime, buffer, 10);
    printf("Time taken : %s\n", buffer);
}

static void print_current_speed(void)
{
    unsigned char status;

    status = ACC_GET_SPEED();
    printf("Current "ACC_NAME" speed : %d\n", status + 1);
}

void main(void)
{
    unsigned char status;
    unsigned char speed = 0;

    status = ACC_DETECT();
    clrscr();
    if (status == 0) {
        printf("No "ACC_NAME" detected\n");
    } else {
        status = ACC_GET_SPEED();
        print_current_speed();

        /* cycle through all the speeds */
        for (speed = SPEED_1X; speed <= SPEED_20X; ++speed) {
            printf("Setting "ACC_NAME" speed to %d\n", speed + 1);
            ACC_SET_SPEED(speed);
            print_current_speed();
            print_time_taken();
        }
        ACC_SET_SPEED(status);
    }
}
