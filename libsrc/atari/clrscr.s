;
; Christian Groessler, Apr-2000
;
; void clrscr (void);
;

        .export         _clrscr
        .include        "atari.inc"
        .importzp       ptr1
        .import         setcursor

SCRSIZE =       960             ; 40x24: size of default atari screen

_clrscr:lda     SAVMSC          ; screen memory
        sta     ptr1
        lda     SAVMSC+1
        clc
        adc     #>(SCRSIZE-1)
        sta     ptr1+1
        lda     #0              ; screen code of space char
        sta     OLDCHR
        ldy     #<(SCRSIZE-1)
        ldx     #>(SCRSIZE-1)
_clr1:  sta     (ptr1),y
        dey
        bne     _clr1
        sta     (ptr1),y
        dex
        bmi     done
        dec     ptr1+1
        dey
        jmp     _clr1

done:   sta     COLCRS
        sta     ROWCRS
        jmp     setcursor
