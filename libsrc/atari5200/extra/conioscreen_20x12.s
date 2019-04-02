; setup alternative CONIO screen (20x12, Antic mode 7, BASIC mode 2)
;
; 02-Apr-2019, Christian Groessler <chris@groessler.org>

                .include "atari5200.inc"

SCREEN_BUF_SIZE =       20 * 12
SCREEN_BUF      =       $4000 - SCREEN_BUF_SIZE

                .export screen_setup
                .export screen_width, screen_height
                .export conio_color

screen_width    =       20
screen_height   =       12


                .segment "ONCE"

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

                .data

conio_color:    .byte   0

                .segment "DLIST"

; display list for 20x12 text mode

dlist:          .repeat 3
                .byte   DL_BLK8
                .endrepeat
                
                .byte   DL_CHR20x16x2 | DL_LMS
                .word   SCREEN_BUF

                .repeat 11
                .byte   DL_CHR20x16x2
                .endrepeat

                .byte  DL_JVB
                .word   dlist

; end of display list

.assert ((* >> 10) = (dlist >> 10)), error, "Display list crosses 1K boundary"

                .end
