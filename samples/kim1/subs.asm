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

count   = $06
count_lo = count
count_hi = count+1

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
                ; load the source (A140) and destination (A000) addresses 
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
                lda (src_lo),y
                sta (dest_lo),y
                iny
                bne :-

                inc dest_hi
                inc src_hi
                ldx src_hi
                cpx #$BF
                bne :-

                lda #$00                    ; Clear the last line (320 bytes, or A0 twice)
                ldy #$A0
:               sta SCREEN+$1EA0, y        
                sta SCREEN+$1E00, y
                dey
                bne :-
                rts




