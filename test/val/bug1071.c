
/* test related to issue #1071 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

struct ImageStruct
{
        uint8_t _imageData;
        #if !defined(NO_COLOR)
                uint8_t _color;
        #endif
};

typedef struct ImageStruct Image;

struct CharacterStruct
{
        // character coordinates
        uint8_t _x;
        uint8_t _y;

        // _status decides whether the character is active
        uint8_t _status;

        Image* _imagePtr;
};

typedef struct CharacterStruct Character;

uint16_t ghostLevel;
uint8_t level;
uint16_t loop;

#define GHOSTS_NUMBER 10
#define BOMBS_NUMBER 3

#define MAX_GHOST_LEVEL_SCALE 3
#define MAX_GHOST_LEVEL (1400/MAX_GHOST_LEVEL_SCALE)

#define MAX_GHOST_LOOP_SCALE 3
#define MAX_GHOST_LOOP (1800/MAX_GHOST_LOOP_SCALE)

#define INITIAL_GHOST_SLOWDOWN 16000

#define ACTION_GHOST_MIN_SLOWDOWN_SCALE 1
#define GHOST_MIN_SLOWDOWN_SCALE ACTION_GHOST_MIN_SLOWDOWN_SCALE
#define GHOST_MIN_SLOWDOWN (3000/GHOST_MIN_SLOWDOWN_SCALE)

Character ghosts[GHOSTS_NUMBER];
Character bombs[BOMBS_NUMBER];

uint16_t test1(void)
{
        if((loop<MAX_GHOST_LOOP) && (ghostLevel<MAX_GHOST_LEVEL))
        {
                return INITIAL_GHOST_SLOWDOWN-(uint16_t)level*256-ghostLevel*8;
        }
        return GHOST_MIN_SLOWDOWN;
}

uint16_t test2(void)
{
        if((loop<MAX_GHOST_LOOP) && (ghostLevel<MAX_GHOST_LEVEL))
        {
                return INITIAL_GHOST_SLOWDOWN-(uint16_t)level*256-ghostLevel*16;
        }
        return GHOST_MIN_SLOWDOWN;
}

uint16_t res = 0;

int main(void)
{
    loop = 7;
    ghostLevel = 13;
    level = 3;

    res = test1();
    printf("test1 res: %d\n", res);
    if (res != 15128) return -1;
    res = test2();
    printf("test2 res: %d\n", res);
    if (res != 15024) return -1;
    return 0;
}
