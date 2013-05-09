;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001
; 06.03.2002
; 25.07.2005

; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);

; TODO:
; TAB (should be implemented)
; other special characters directly from keyboard are unsafe, though some might be
; implemented:
; HOME, UPLINE, ULINEON, ULINEOFF, REV_ON, REV_OFF, BOLDON, ITALICON, OUTLINEON, PLAINTEXT
; and cursor movement, maybe stuff like INSERT too
;
; these must be ignored:
; ESC_GRAPHICS, ESC_RULER, GOTOX, GOTOY, GOTOXY, NEWCARDSET, all 1..8
;
; note that there are conflicts between control characters and keyboard:
; HOME = KEY_ENTER, KEY_HOME = REV_ON, 
; UPLINE = ?, KEY_UPARROW = GOTOY, ...

            .export _cputcxy, _cputc
            .import _gotoxy, fixcursor
            .import popa
            .import xsize,ysize
            .importzp cursor_x, cursor_y, cursor_c, cursor_r

            .include "const.inc"
            .include "geossym.inc"
            .include "jumptab.inc"

_cputcxy:
        pha                     ; Save C
        jsr popa                ; Get Y
        jsr _gotoxy             ; Set cursor, drop x
        pla                     ; Restore C

; Plot a character - also used as internal function

_cputc:
        tax                     ; save character
; some characters 0-31 are not safe for PutChar
        cmp #$20
        bcs L1
        cmp #CR
        beq do_cr
        cmp #LF
        beq do_lf
        cmp #KEY_DELETE
        bne L0
        ldx #BACKSPACE
        sec
        bcs L2
L0:     rts

L1:     clc
L2:     php
        lda cursor_x
        sta r11L
        lda cursor_x+1
        sta r11H
        lda cursor_y
        clc
        adc #6                  ; 6 pixels down to the baseline
        sta r1H
        txa
        jsr PutChar
        plp
        bcs fix_cursor

        inc cursor_c
        lda cursor_c
        cmp xsize               ; hit right margin?
        bne fix_cursor
        lda #0                  ; yes - do cr+lf
        sta cursor_c
do_lf:  inc cursor_r
        lda cursor_r
        cmp ysize               ; hit bottom margin?
        bne fix_cursor
        dec cursor_r            ; yes - stay in the last line

fix_cursor:
        jmp fixcursor

do_cr:  lda #0
        sta cursor_c
        beq fix_cursor
