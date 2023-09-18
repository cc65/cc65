;-----------------------------------------------------------------------------------
; KIMGFX: Simple pixel graphics for the MOS/Commodore KIM-1
;-----------------------------------------------------------------------------------
; (c) Plummer's Software Ltd, 04/25/2023 Created
;         David Plummer
;-----------------------------------------------------------------------------------
;
; File:     subs.asm            Assembly language subroutines for KIMGFX
;
;-----------------------------------------------------------------------------------


.SETCPU "6502"

.export _ClearScreen
.export _ScrollScreen

SCREEN  = $A000

dest    = $02
dest_lo = dest
dest_hi = dest+1

src    = $04
src_lo = src
src_hi = src+1

_ClearScreen:
                lda #$00

                ldx #<SCREEN
                stx dest_lo
                ldx #>SCREEN
                stx dest_hi

                ldy #0
:               sta (dest), y
                iny
                bne :-

                inc dest_hi
                ldx dest_hi
                cpx #>SCREEN + $20
                bne :-

                rts

_ScrollScreen:
                ; Load the source (A140) and destination (A000) addresses.  Each row of characters
                ; occupies 320 bytes, so we start source as being one line ahead of the destination
                ; which will have the effect of scrolling the screen up one text line.

                lda #<(SCREEN+320)
                sta src_lo
                lda #>(SCREEN+320)
                sta src_hi
                lda #<SCREEN
                sta dest_lo
                lda #>SCREEN
                sta dest_hi

                ldy #$00
:
                lda (src),y
                sta (dest),y
                iny
                bne :-

                inc dest_hi                 ; When the source hits $BF00 we're done, as we've copied
                inc src_hi                  ;   everything up to that point so far
                ldx src_hi
                cpx #$BE
                bne :-

                lda #$00                    ; Clear the last line (320 bytes, or A0 twice)
                ldy #$A0
:               sta SCREEN+$1EA0, y
                sta SCREEN+$1E00, y
                dey
                bne :-
                rts




