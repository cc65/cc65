;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001
; 06.03.2002

; void gotox (unsigned char x);
; void gotoy (unsigned char y);
; void gotoxy (unsigned char x, unsigned char y);

            .export _gotox, _gotoy, _gotoxy, fixcursor
            .import popa
            .importzp cursor_x, cursor_y, cursor_c, cursor_r

            .include "jumptab.inc"

_gotox:
        sta cursor_c
        jmp fixcursor

_gotoy:
        sta cursor_r
        jmp fixcursor

_gotoxy:
        sta cursor_r
        jsr popa
        sta cursor_c

; convert 8x8 x/y coordinates to GEOS hires
fixcursor:
        lda cursor_c
        sta cursor_x
        lda #0
        sta cursor_x+1
        ldx #cursor_x
        ldy #3
        jsr DShiftLeft
        lda cursor_r
        asl a
        asl a
        asl a
        sta cursor_y
        rts
