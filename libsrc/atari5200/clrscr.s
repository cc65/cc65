;
; Christian Groessler, May-2014
;
; void clrscr (void);
;

        .export         _clrscr
        .include        "atari5200.inc"
        .importzp       ptr1

SCRSIZE =       480             ; 20x24: size of default conio atari5200 screen

_clrscr:lda     SAVMSC          ; screen memory
        sta     ptr1
        lda     SAVMSC+1
        clc
        adc     #>(SCRSIZE-1)
        sta     ptr1+1
        lda     #0              ; screen code of space char
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

done:   sta     COLCRS_5200
        sta     ROWCRS_5200
        rts
