
/* #1652 - Regression in code optimization */

#include <stdlib.h>

/* better before pr#1231

before:

        jsr     pusha
        ldy     #$01
        ldx     #$00
        lda     (c_sp),y
        beq     L0005
        lda     (c_sp,x)
        beq     L0005
        txa
        jmp     incsp2          ; <--
L0005:  tya
        jmp     incsp2

after:

        jsr     pusha
        ldy     #$01
        ldx     #$00
        lda     (c_sp),y
        beq     L0004
        lda     (c_sp,x)
        beq     L0004
        txa
        jmp     L0001            ; <--
L0004:  tya
L0001:  jmp     incsp2

*/

int foo(unsigned char bar, unsigned char baz)
{
    return (bar == 0 || baz == 0);
}

/* worse before pr#1231

before:
;
; y = 1;
;
    lda     #$01
    sta     _y
;
; (*tablePtr)[y][x] = 83;
;
    ldx     #$00
    lda     _y
    jsr     aslax2
    clc
    adc     _tablePtr
    pha
    txa
    adc     _tablePtr+1
    tax
    pla
    clc
    adc     _x
    bcc     L0019
    inx
L0019:
    sta     ptr1
    stx     ptr1+1
    lda     #$53
    ldy     #$00
    sta     (ptr1),y

after:

;
; y = 1;
;
    lda     #$01
    sta     _y
;
; (*tablePtr)[y][x] = 83;
;
    ldx     #$00
    lda     _y
    jsr     aslax2
    clc
    adc     _tablePtr
    sta     ptr1
    txa
    adc     _tablePtr+1
    sta     ptr1+1
    ldy     _x
    lda     #$53
    sta     (ptr1),y
*/

typedef unsigned char array_t[4][4];

static array_t table = {
    {12, 13, 14, 15},
    { 8,  9, 10, 11},
    { 4,  5,  6,  7},
    { 0,  1,  2,  3}
};
static array_t *tablePtr = &table;

static unsigned char y = 0, x;

void pointed_array(void)
{
    x = 3;
    y = 1;
    (*tablePtr)[y][x] = 83;
}

int main(void)
{
    pointed_array();
    return foo(2, 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
