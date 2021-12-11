; setup default CONIO screen (20x24, Antic mode 6, BASIC mode 1)
;
; 28-May-2014, Christian Groessler <chris@groessler.org>

                .include "atari5200.inc"

SCREEN_BUF_SIZE =       20 * 24
SCREEN_BUF      =       $4000 - SCREEN_BUF_SIZE

                .export screen_setup
                .export screen_width, screen_height
                .export conio_color

screen_width    =       20
screen_height   =       24


                .segment "ONCE"

; initialize color registers, display list, and screen memory
screen_setup:

                ; initialize SAVMSC
                lda     #<SCREEN_BUF
                sta     SAVMSC
                lda     #>SCREEN_BUF
                sta     SAVMSC+1

                ; initialize cursor position
                lda     #0
                sta     COLCRS_5200
                sta     ROWCRS_5200

                ; clear screen buffer
                ldy     #<(SCREEN_BUF_SIZE-1)
                ldx     #>(SCREEN_BUF_SIZE-1)
clrscr:         sta     (SAVMSC),y
                dey
                cpy     #$FF
                bne     clrscr
                dex
                cpx     #$FF
                bne     clrscr

                ; set default colors

                lda     #GTIA_COLOR_WHITE
                sta     COLOR0
                lda     #GTIA_COLOR_LIGHTRED
                sta     COLOR1
                lda     #GTIA_COLOR_LIGHTGREEN
                sta     COLOR2
                lda     #GTIA_COLOR_BLACK
                sta     COLOR3
                sta     COLOR4          ; background

                ; set display list

                lda     #<dlist
                sta     SDLSTL
                lda     #>dlist
                sta     SDLSTH

                rts

                .bss

conio_color:    .res    1

                .segment "DLIST"

; display list for 20x24 text mode

dlist:          .repeat 3
                .byte   DL_BLK8
                .endrepeat
                
                .byte   DL_CHR20x8x2 | DL_LMS
                .word   SCREEN_BUF

                .repeat 23
                .byte   DL_CHR20x8x2
                .endrepeat

                .byte  DL_JVB
                .word   dlist

; end of display list

.assert ((* >> 10) = (dlist >> 10)), error, "Display list crosses 1K boundary"

                .end
