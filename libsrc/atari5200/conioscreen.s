; setup default CONIO screen (20x24, Antic mode 6, BASIC mode 1)
;
; 28-May-2014, Christian Groessler <chris@groessler.org>

                .include "atari5200.inc"

SCREEN_BUF_SIZE =       20 * 24
SCREEN_BUF      =       $4000 - SCREEN_BUF_SIZE

                .export screen_setup_20x24

                .segment "ONCE"

screen_setup_20x24:

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

                lda     #40
                sta     COLOR0
                lda     #202
                sta     COLOR1
                lda     #148
                sta     COLOR2
                lda     #70
                sta     COLOR3
                lda     #0
                sta     COLOR4

                ; set display list

                lda     #<dlist
                sta     SDLSTL
                lda     #>dlist
                sta     SDLSTH

                rts


                .segment "RODATA"

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
