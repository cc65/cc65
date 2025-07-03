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
.export _SetPixel
.export _ClearPixel
.export _DrawCircle
.export _DrawLine
.export _AscToPet
.export _ReverseBits
.export _DrawChar
.export _CharOut
.export _Demo
.export _Delay
.export _getch


.import _font8x8_basic

; This is the assumed location of the MTU visible memory board's 8K of memory.  You can adjust this
; constant to refelct other locations as needed.

SCREEN           = $A000

; Note that even though these constants are defined here and respected, there are still going to be
; logic assumptions in GetPixelAddress that assume a 320x200 screen.  If you change these, you'll
; need to adjust GetPixelAddress to match.

SCREEN_WIDTH     = 320
SCREEN_HEIGHT    = 200
SCREEN_BYTES     = SCREEN_WIDTH * SCREEN_HEIGHT / 8
CHARWIDTH        = 8
CHARHEIGHT       = 8
BYTESPERROW      = (SCREEN_WIDTH / 8)
BYTESPERCHARROW  = (BYTESPERROW * 8)
CHARSPERROW      = (SCREEN_WIDTH / CHARWIDTH)
ROWSPERCOLUMN    = (SCREEN_HEIGHT / CHARHEIGHT)
LASTROW          = SCREEN + SCREEN_BYTES - BYTESPERCHARROW

.segment "ZEROPAGE"

btpt:           .res 1

dest:
dest_lo:        .res 1
dest_hi:        .res 1

src:
src_lo:         .res 1
src_hi:         .res 1

adp1:
adp1_lo:        .res 1
adp1_hi:        .res 1

adp2:
adp2_lo:        .res 1
adp2_hi:        .res 1

scroll_src:
scroll_src_lo:  .res 1
scroll_src_hi:  .res 1

scroll_dest:
scroll_dest_lo: .res 1
scroll_dest_hi: .res 1


.segment "DATA"

; Arguments for graphics functions

_x1cord:        .res 2
_x2cord:        .res 2
_y1cord:        .res 2
_y2cord:        .res 2
_cursorX:       .res 1
_cursorY:       .res 1

; Linedraw

dx:         .res 2
dy:         .res 2
e2:         .res 2
sx:         .res 1
sy:         .res 1
dltemp:     .res 2
pixel:      .res 1

; DrawCircle

xval:           .res 2              ; These could move to zeropage for perf, but presume we
yval:           .res 2              ;   we want to minimize the amount we grow zero page use
err:            .res 2
temp:           .res 2
tempa:          .res 1
tempx:          .res 1
tempy:          .res 1
temp2:          .res 2
x0:             .res 2
y0:             .res 2

; CharOut

tempstr:        .res 2

.export _x1cord                   ; Make sure these show up on the C side as zero page
.export _x2cord
.export _y1cord
.export _y2cord
.export _cursorX
.export _cursorY

.segment "CODE"

;-----------------------------------------------------------------------------------
; GetPixelAddress - Calculate the address of a pixel in the video memory
;-----------------------------------------------------------------------------------
; Based on MTU PIXADR code
;-----------------------------------------------------------------------------------
; In:       _x1cord         (16-bit)
;           _y1cord         (16-bit)
; Out:      adp1            (16-bit) Address of pixel to set
;-----------------------------------------------------------------------------------

_GetPixelAddress:
                lda     _x1cord         ; compute bit address first
                sta     adp1            ; also transfer x1cord to adp1
                and     #$07            ; + which is simply the low 3 bits of x
                sta     btpt
                lda     _x1cord+1       ; finish transferring x1cord to adp1
                sta     adp1+1
                lsr     adp1+1          ; double shift adp1 right 3 to get
                ror     adp1            ; int(xcord/8 )
                lsr     adp1+1
                ror     adp1
                lsr     adp1+1
                ror     adp1
                sec                             ; and temporary storage
                lda     _y1cord
                sta     adp2
                sta     temp
                lda     #0
                sbc     _y1cord+1
                sta     adp2+1
                sta     temp+1
                asl     adp2            ; compute 40*(y1cord)
                rol     adp2+1          ;  2*(y1cord)
                asl     adp2
                rol     adp2+1          ;  4*(y1cord)
                lda     adp2            ;  add in temporary save of (y1cord)
                clc                             ;  to make 5*(y1cord)
                adc     temp
                sta     adp2
                lda     adp2+1
                adc     temp+1
                sta     adp2+1          ; 5*(y1cord)
                asl     adp2            ; 10*(1cord)
                rol     adp2+1
                asl     adp2            ; 20#(y1cord)
                rol     adp2+1
                asl     adp2            ; 40*(y1cord)
                rol     adp2+1
                lda     adp2            ; add in int(x1cord/8) computed earlier
                clc
                adc     adp1
                sta     adp1
                lda     adp2+1
                adc     adp1+1
                adc     #>SCREEN    ; add in vmorg*256
                sta     adp1+1          ; final result
                rts                             ; return

;-----------------------------------------------------------------------------------
; Mask tables for individual pixel subroutines
;
; MSKTB1 is a table of 1 bits corresponding to bit numbers
; MSKTB2 is a table of 0 bits corresponding to bit numbers
;-----------------------------------------------------------------------------------

msktb1:             .byte       $80,$40,$20,$10,$08,$04,$02,$01
msktb2:             .byte       $7F,$BF,$DF,$EF,$F7,$FB,$FD,$FE

_Delay:         pha
                sta temp
                txa
                pha
                tya
                pha

@loopa:         ldx #$ff
@loopx:         ldy #$ff
@loopy:         dey
                bne @loopy
                dex
                bne @loopx
                dec temp
                bne @loopa

                pla
                tay
                pla
                tax
                pla
                rts

;-----------------------------------------------------------------------------------
; SetPixel - Set a pixel in the video memory
;-----------------------------------------------------------------------------------
; x             - _x1cord (16-bit)
; y             - _y1cord (16-bit)
;-----------------------------------------------------------------------------------
; Mask tables for individual pixel subroutines
;-----------------------------------------------------------------------------------

_SetPixel:      jsr     _GetPixelAddress
                ldy     btpt            ; get bit number in y
                lda     msktb1,y        ; get a byte with that bit =1, others =0
                ldy     #0
                ora     (adp1),y        ; combine the bit with the addressed vm
                sta     (adp1),y        ; byte
                rts

;-----------------------------------------------------------------------------------
; ClearPixel - Clears a pixel in the video memory
;-----------------------------------------------------------------------------------
; x             - _x1cord (16-bit)
; y             - _y1cord (16-bit)
;-----------------------------------------------------------------------------------

_ClearPixel:    jsr     _GetPixelAddress
                ldy     btpt            ; get bit number in y
                lda     msktb2,y        ; get a byte with that bit =0, others =1
                ldy     #0
                and     (adp1),y        ; remove the bit from the addressed vm
                sta     (adp1),y        ; byte
                rts

;-----------------------------------------------------------------------------------
; ClearScreen - Clears the entire video memory (and thus the screen)
;-----------------------------------------------------------------------------------

_ClearScreen:
                lda #$00

                ldx #<SCREEN
                stx dest_lo
                ldx #>SCREEN
                stx dest_hi

                ldy #0
:               sta (dest), y       ; Loop unwound by a factor of 8, which means our iny before the branchh
                iny                 ;  will still work as it's on a page crossing boundary.
                sta (dest), y       ;  This will avoid most of the overhead of the branch.
                iny
                sta (dest), y
                iny
                sta (dest), y
                iny
                sta (dest), y
                iny
                sta (dest), y
                iny
                sta (dest), y
                iny
                sta (dest), y
                iny
                bne :-

                inc dest_hi
                ldx dest_hi
                cpx #>SCREEN + $20
                bne :-

                rts

;-----------------------------------------------------------------------------------
; ScrollScreen - Scrolls the entire video memory (and thus the screen) up one row
;-----------------------------------------------------------------------------------

BYTES_TO_MOVE      = SCREEN_BYTES - BYTESPERCHARROW
PAGES_TO_MOVE      = BYTES_TO_MOVE / 256

_ScrollScreen:
                pha
                tya
                pha
                txa
                pha

                ; Load the source (A140) and destination (A000) addresses
                lda #<(SCREEN+BYTESPERCHARROW)
                sta scroll_src_lo
                lda #>(SCREEN+BYTESPERCHARROW)
                sta scroll_src_hi
                lda #<SCREEN
                sta scroll_dest_lo
                lda #>SCREEN
                sta scroll_dest_hi

                ldx #PAGES_TO_MOVE
@outerLoop:
                ldy #0
@innerLoop:     ;
                ; I could do this faster in self-modifying code (avoiding the zero page overhead) but then it
                ; couldn't go into ROM

                lda (scroll_src),y                          ; I've unwound the loop to do 8 bytes at a time.  Since we're doing full pages
                sta (scroll_dest),y                         ;   as long as we unwind the loop to do 8 bytes at a time, we know we'll still
                iny                                         ;   do the final increment on a page boundary.
                lda (scroll_src),y
                sta (scroll_dest),y
                iny
                lda (scroll_src),y
                sta (scroll_dest),y
                iny
                lda (scroll_src),y
                sta (scroll_dest),y
                iny
                lda (scroll_src),y
                sta (scroll_dest),y
                iny
                lda (scroll_src),y
                sta (scroll_dest),y
                iny
                lda (scroll_src),y
                sta (scroll_dest),y
                iny
                lda (scroll_src),y
                sta (scroll_dest),y
                iny
                bne @innerLoop                              ; If Y overflows, it will be 0, so won't branch
                inc scroll_src_hi
                inc scroll_dest_hi
                dex
                bne @outerLoop

                ; Clear the last line
                lda #<LASTROW
                sta scroll_dest_lo
                lda #>LASTROW
                sta scroll_dest_hi
                lda #$00
                ldy #0
fullPageLoop:
                sta (scroll_dest_lo),y
                iny
                bne fullPageLoop
                inc scroll_dest_hi
partialPageLoop:
                sta (scroll_dest_lo),y
                iny
                cpy #BYTESPERCHARROW - 256         ; Only clear up to the 64th byte (256 + 64 == 320)
                bne partialPageLoop

                pla
                tax
                pla
                tay
                pla
                rts

;-----------------------------------------------------------------------------------
; DrawCircle    - Draws a circle in video memory of a given radius at a given coord
;-----------------------------------------------------------------------------------
; x             - _x1cord (16-bit)
; y             - _y1cord (16-bit)
; radius        - _y2cord (16-bit)
;-----------------------------------------------------------------------------------
; Implements the midpoint circle algorithm without floating point or trig functions
;-----------------------------------------------------------------------------------
;     int x = radius;
;     int y = 0;
;     int err = 0;
;
;     while (x >= y)
;     {
;         SETPIXEL(x0 + x, y0 + y, val);
;         SETPIXEL(x0 + y, y0 + x, val);
;         SETPIXEL(x0 - y, y0 + x, val);
;         SETPIXEL(x0 - x, y0 + y, val);
;         SETPIXEL(x0 - x, y0 - y, val);
;         SETPIXEL(x0 - y, y0 - x, val);
;         SETPIXEL(x0 + y, y0 - x, val);
;         SETPIXEL(x0 + x, y0 - y, val);
;
;         y++;
;         err += 1 + 2 * y;
;         if (2 * (err - x) + 1 > 0) {
;             x--;
;             err += 1 - 2 * x;
;         }
;     }
;-----------------------------------------------------------------------------------

_DrawCircle:    lda _x1cord                     ; x0 = _x1cord
                sta x0
                lda _x1cord+1
                sta x0+1
                lda _y1cord                     ; y0 = _y1cord
                sta y0
                lda _y1cord+1
                sta y0+1

                lda _y2cord                     ; x = radius
                sta xval
                lda _y2cord+1
                sta xval+1

                lda #$0                         ; yval = 0;
                sta yval
                sta yval+1
                sta err                         ; err = 0;
                sta err+1
circleloop:
                lda xval+1                      ; if (xval < yval) we're done;
                sec
                cmp yval+1
                bcc doneCircle                  ; if high byteof yval is greater, we can draw
                bne doCircle                    ; it not greater and not equal, is less, so done
                lda xval                        ; in other cases we need to compare the LSB next
                cmp yval
                bcs doCircle                    ; if it's less, but MSB was equal, we go draw

doneCircle:     rts

doCircle:       lda x0                          ; Draw the first of 8 symmetric quadrant copies
                clc
                adc yval
                sta _x1cord
                lda x0+1
                adc yval+1
                sta _x1cord+1
                lda y0
                sec
                sbc xval
                sta _y1cord
                lda y0+1
                sbc xval+1
                sta _y1cord+1
                jsr _SetPixel                    ; SETPIXEL(x0 + y, y0 - x, val);

                lda x0
                sec
                sbc yval
                sta _x1cord
                lda x0+1
                sbc yval+1
                sta _x1cord+1
                lda y0
                sec
                sbc xval
                sta _y1cord
                lda y0+1
                sbc xval+1
                sta _y1cord+1
                jsr _SetPixel                    ; SETPIXEL(x0 - y, y0 - x, val);

                lda x0
                sec
                sbc xval
                sta _x1cord
                lda x0+1
                sbc xval+1
                sta _x1cord+1
                lda y0
                sec
                sbc yval
                sta _y1cord
                lda y0+1
                sbc yval+1
                sta _y1cord+1
                jsr _SetPixel                    ; SETPIXEL(x0 - x, y0 - y, val);

                lda x0
                sec
                sbc xval
                sta _x1cord
                lda x0+1
                sbc xval+1
                sta _x1cord+1
                lda y0
                clc
                adc yval
                sta _y1cord
                lda y0+1
                adc yval+1
                sta _y1cord+1
                jsr _SetPixel                    ; SETPIXEL(x0 - x, y0 + y, val);

                lda x0
                clc
                adc yval
                sta _x1cord
                lda x0+1
                adc yval+1
                sta _x1cord+1
                lda y0
                clc
                adc xval
                sta _y1cord
                lda y0+1
                adc xval+1
                sta _y1cord+1
                jsr _SetPixel                    ; SETPIXEL(x0 + y, y0 + x, val);

                lda x0
                clc
                adc xval
                sta _x1cord
                lda x0+1
                adc xval+1
                sta _x1cord+1
                lda y0
                clc
                adc yval
                sta _y1cord
                lda y0+1
                adc yval+1
                sta _y1cord+1
                jsr _SetPixel                    ; SETPIXEL(x0 + x, y0 + y, val);

                lda x0
                clc
                adc xval
                sta _x1cord
                lda x0+1
                adc xval+1
                sta _x1cord+1
                lda y0
                sec
                sbc yval
                sta _y1cord
                lda y0+1
                sbc yval+1
                sta _y1cord+1
                jsr _SetPixel                    ; SETPIXEL(x0 + x, y0 - y, val);

                lda x0
                sec
                sbc yval
                sta _x1cord
                lda x0+1
                sbc yval+1
                sta _x1cord+1
                lda y0
                clc
                adc xval
                sta _y1cord
                lda y0+1
                adc xval+1
                sta _y1cord+1
                jsr _SetPixel                   ; SETPIXEL(x0 - y, y0 + x, val);

                inc yval                        ; yval++;
                bne :+
                inc yval+1

:               lda yval                        ; temp = 2 * yval + 1;
                asl
                sta temp
                lda yval+1
                rol
                sta temp+1
                inc temp
                bne :+
                inc temp+1
:
                lda err                         ; err += temp
                clc
                adc temp
                sta err
                lda err+1
                adc temp+1
                sta err+1
                                                ; if (2 * (err - xval) + 1 > 0) then dec xval
                lda err                         ; temp = err-xval
                sec
                sbc xval
                sta temp
                lda err+1
                sbc xval+1
                sta temp+1

                asl temp                        ; temp = 2*(err-xval)+1
                rol temp+1
                inc temp
                bne :+
                inc temp+1
:
                lda temp+1                      ; if (temp > 0) we'll dec xval
                bmi doneLoop                    ; less than zero, so no dec
                bne decxval                     ; if not zero, go ahead and dec

                lda temp                        ; MSB is zero so now check the LSB
                beq doneLoop                    ; both bytes are zero, so no dec

decxval:        lda xval                        ; xval--
                bne :+
                dec xval+1
:               dec xval

updateerr:      lda xval                        ; temp = xval * 2
                asl
                sta temp
                lda xval+1
                rol
                sta temp+1

                lda #1                          ; temp2 == 1-temp == 1-(xval*2)
                sec
                sbc temp
                sta temp2
                lda #0
                sbc temp+1
                sta temp2+1

                lda err                         ; err += 1-(xval*2)
                clc
                adc temp2
                sta err
                lda err+1
                adc temp2+1
                sta err+1

doneLoop:       jmp circleloop

;-----------------------------------------------------------------------------------
; Character set translation tables
;-----------------------------------------------------------------------------------

ascToPetTable:  .byte $00,$01,$02,$03,$04,$05,$06,$07,$14,$20,$0d,$11,$93,$0a,$0e,$0f
                .byte $10,$0b,$12,$13,$08,$15,$16,$17,$18,$19,$1a,$1b,$1c,$1d,$1e,$1f
                .byte $20,$21,$22,$23,$24,$25,$26,$27,$28,$29,$2a,$2b,$2c,$2d,$2e,$2f
                .byte $30,$31,$32,$33,$34,$35,$36,$37,$38,$39,$3a,$3b,$3c,$3d,$3e,$3f
                .byte $40,$c1,$c2,$c3,$c4,$c5,$c6,$c7,$c8,$c9,$ca,$cb,$cc,$cd,$ce,$cf
                .byte $d0,$d1,$d2,$d3,$d4,$d5,$d6,$d7,$d8,$d9,$da,$5b,$5c,$5d,$5e,$5f
                .byte $c0,$41,$42,$43,$44,$45,$46,$47,$48,$49,$4a,$4b,$4c,$4d,$4e,$4f
                .byte $50,$51,$52,$53,$54,$55,$56,$57,$58,$59,$5a,$db,$dc,$dd,$de,$df
                .byte $80,$81,$82,$83,$84,$85,$86,$87,$88,$89,$8a,$8b,$8c,$8d,$8e,$8f
                .byte $90,$91,$92,$0c,$94,$95,$96,$97,$98,$99,$9a,$9b,$9c,$9d,$9e,$9f
                .byte $a0,$a1,$a2,$a3,$a4,$a5,$a6,$a7,$a8,$a9,$aa,$ab,$ac,$ad,$ae,$af
                .byte $b0,$b1,$b2,$b3,$b4,$b5,$b6,$b7,$b8,$b9,$ba,$bb,$bc,$bd,$be,$bf
                .byte $60,$61,$62,$63,$64,$65,$66,$67,$68,$69,$6a,$6b,$6c,$6d,$6e,$6f
                .byte $70,$71,$72,$73,$74,$75,$76,$77,$78,$79,$7a,$7b,$7c,$7d,$7e,$7f
                .byte $e0,$e1,$e2,$e3,$e4,$e5,$e6,$e7,$e8,$e9,$ea,$eb,$ec,$ed,$ee,$ef
                .byte $f0,$f1,$f2,$f3,$f4,$f5,$f6,$f7,$f8,$f9,$fa,$fb,$fc,$fd,$fe,$ff

; PETSCI to Ascii lookup table - not current used, so commented out, but can be used to map fonts
;
;
 petToAscTable:  .byte $00,$01,$02,$03,$04,$05,$06,$07,$14,$09,$0d,$11,$93,$0a,$0e,$0f
                 .byte $10,$0b,$12,$13,$08,$15,$16,$17,$18,$19,$1a,$1b,$1c,$1d,$1e,$1f
                 .byte $20,$21,$22,$23,$24,$25,$26,$27,$28,$29,$2a,$2b,$2c,$2d,$2e,$2f
                 .byte $30,$31,$32,$33,$34,$35,$36,$37,$38,$39,$3a,$3b,$3c,$3d,$3e,$3f
                 .byte $40,$61,$62,$63,$64,$65,$66,$67,$68,$69,$6a,$6b,$6c,$6d,$6e,$6f
                 .byte $70,$71,$72,$73,$74,$75,$76,$77,$78,$79,$7a,$5b,$5c,$5d,$5e,$5f
                 .byte $c0,$c1,$c2,$c3,$c4,$c5,$c6,$c7,$c8,$c9,$ca,$cb,$cc,$cd,$ce,$cf
                 .byte $d0,$d1,$d2,$d3,$d4,$d5,$d6,$d7,$d8,$d9,$da,$db,$dc,$dd,$de,$df
                 .byte $80,$81,$82,$83,$84,$85,$86,$87,$88,$89,$8a,$8b,$8c,$8d,$8e,$8f
                 .byte $90,$91,$92,$0c,$94,$95,$96,$97,$98,$99,$9a,$9b,$9c,$9d,$9e,$9f
                 .byte $a0,$a1,$a2,$a3,$a4,$a5,$a6,$a7,$a8,$a9,$aa,$ab,$ac,$ad,$ae,$af
                 .byte $b0,$b1,$b2,$b3,$b4,$b5,$b6,$b7,$b8,$b9,$ba,$bb,$bc,$bd,$be,$bf
                 .byte $60,$41,$42,$43,$44,$45,$46,$47,$48,$49,$4a,$4b,$4c,$4d,$4e,$4f
                 .byte $50,$51,$52,$53,$54,$55,$56,$57,$58,$59,$5a,$7b,$7c,$7d,$7e,$7f
                 .byte $a0,$a1,$a2,$a3,$a4,$a5,$a6,$a7,$a8,$a9,$aa,$ab,$ac,$ad,$ae,$af
                 .byte $b0,$b1,$b2,$b3,$b4,$b5,$b6,$b7,$b8,$b9,$ba,$bb,$bc,$bd,$be,$bf

;-----------------------------------------------------------------------------------
; PetToAsc      - Convert a PETSCII character to ASCII
;-----------------------------------------------------------------------------------
; A             - Character to convert
;-----------------------------------------------------------------------------------

_AscToPet:      tay
                lda ascToPetTable, y
                rts

_PetToAsc:      tay
                lda petToAscTable, Y
                rts

;-----------------------------------------------------------------------------------
; ReverseBits   - Reverse the bits in a byte
;-----------------------------------------------------------------------------------
; A = octet to be reversed
;-----------------------------------------------------------------------------------

_ReverseBits:
                ldx #8           ; set counter to 8 (for 8 bits)
                lda #0           ; initialize A to 0
                sta temp         ; clear result byte (accumulator for the reversed octet)
:               asl              ; shift leftmost bit of input into carry
                lda temp         ; load the temporary result byte into A
                ror              ; rotate carry into leftmost bit of result
                sta temp         ; store the updated result back into memory
                dex              ; decrement counter
                bne :-           ; repeat until all bits are processed
                lda temp         ; load the final reversed byte into A
                rts              ; return with result in A

;-----------------------------------------------------------------------------------
; LoadFont      - Makes sure the font data is ready to use.  This usually requires
;                 reversing the bits so that they match the bit order of the screen
;-----------------------------------------------------------------------------------

_LoadFont:      ldx #3
                lda #<_font8x8_basic
                sta adp1_lo
                lda #>_font8x8_basic
                sta adp1_hi
                ldy #0
@loop:          lda (adp1), y
                jsr _ReverseBits
                sta (adp1), y
                iny
                bne @loop

                inc adp1_lo
                bne :+
                inc adp1_hi
:               dex
                bne @loop
                rts

ScreenLineAddresses:
                .word SCREEN +  0 * BYTESPERCHARROW, SCREEN +  1 * BYTESPERCHARROW
                .word SCREEN +  2 * BYTESPERCHARROW, SCREEN +  3 * BYTESPERCHARROW
                .word SCREEN +  4 * BYTESPERCHARROW, SCREEN +  5 * BYTESPERCHARROW
                .word SCREEN +  6 * BYTESPERCHARROW, SCREEN +  7 * BYTESPERCHARROW
                .word SCREEN +  8 * BYTESPERCHARROW, SCREEN +  9 * BYTESPERCHARROW
                .word SCREEN + 10 * BYTESPERCHARROW, SCREEN + 11 * BYTESPERCHARROW
                .word SCREEN + 12 * BYTESPERCHARROW, SCREEN + 13 * BYTESPERCHARROW
                .word SCREEN + 14 * BYTESPERCHARROW, SCREEN + 15 * BYTESPERCHARROW
                .word SCREEN + 16 * BYTESPERCHARROW, SCREEN + 17 * BYTESPERCHARROW
                .word SCREEN + 18 * BYTESPERCHARROW, SCREEN + 19 * BYTESPERCHARROW
                .word SCREEN + 20 * BYTESPERCHARROW, SCREEN + 21 * BYTESPERCHARROW
                .word SCREEN + 22 * BYTESPERCHARROW, SCREEN + 23 * BYTESPERCHARROW
                .word SCREEN + 24 * BYTESPERCHARROW
                .assert( (* - ScreenLineAddresses) = ROWSPERCOLUMN * 2), error

;-----------------------------------------------------------------------------------
; DrawChar     - Draws an ASCII character at char location x, y
;-----------------------------------------------------------------------------------
; 0 <= x < 40
; 0 <= y < 25
; Preserves all registers, but its not very threadsafe or reentrant
;-----------------------------------------------------------------------------------

_DrawChar:      sty tempy
                stx tempx
                sta tempa

                tya                                 ; Get the address in screen memory where this
                asl                                 ;  character X/Y cursor pos should be drawn
                tay
                txa
                clc
                adc ScreenLineAddresses, y
                sta dest_lo
                lda ScreenLineAddresses+1, y
                adc #0
                sta dest_hi

                lda #0                              ; Get the address in font memory where this
                sta src_hi                          ;  Petscii character lives (after conversion from
                lda tempa                           ;  ascii)

                sty temp2
                jsr _AscToPet
                ldy temp2

                asl
                rol src_hi
                asl
                rol src_hi
                asl
                rol src_hi
                clc
                adc #<_font8x8_basic                ; Add the base address of the font table to the offset
                sta src_lo
                lda src_hi
                adc #>_font8x8_basic
                sta src_hi

                ldy #0                              ; opy the character def to the screen, one byte at a time
                ldx #0
:               lda (src), y                        ; Copy this byte from the character def to the screen target
                sta (dest, x)
                lda dest_lo                         ; Advance to the next "scanline", or pixel row, down
                clc
                adc #<BYTESPERROW
                sta dest_lo
                lda dest_hi
                adc #>BYTESPERROW
                sta dest_hi

                iny
                cpy #8
                bne :-

                ldy tempy
                ldx tempx
                lda tempa
                rts

;-----------------------------------------------------------------------------------
; CursorOn     - Turns on the text cursor and draws it at the current cursor pos
;-----------------------------------------------------------------------------------

CursorOn:       ldx _cursorX
                ldy _cursorY
                lda #'_'
                jsr _DrawChar
                rts

CursorOff:      ldx _cursorX
                ldy _cursorY
                lda #' '
                jsr _DrawChar
                rts

;-----------------------------------------------------------------------------------
; DrawText     - Draws an ASCII char in A at the current cursor pos, saves all regs
;-----------------------------------------------------------------------------------

_CharOut:       sta temp
                lda #0
                sta temp+1
                txa
                pha
                tya
                pha

                ldx #<temp
                ldy #>temp
                jsr _DrawText

                pla
                tay
                pla
                tax
                rts

;-----------------------------------------------------------------------------------
; Backspace    - Erase the current character and move back one position. Does not
;                move back up to previous line
;-----------------------------------------------------------------------------------

Backspace:      lda _cursorX
                beq colzero
                jsr CursorOff
                dec _cursorX
                jsr CursorOn
colzero:        rts

;-----------------------------------------------------------------------------------
; DrawText     - Draws an ASCII string at the current cursor position
;-----------------------------------------------------------------------------------
; XY           - Pointer to the string to draw, stops on NUL or 255 chars later
;-----------------------------------------------------------------------------------

_DrawText:      stx adp1_lo
                sty adp1_hi
                jsr CursorOff

                ldy #0
checkHWrap:     lda _cursorX
                cmp #CHARSPERROW
                bcc checkVWrap
                lda #0
                sta _cursorX
                inc _cursorY

checkVWrap:     lda _cursorY
                cmp #ROWSPERCOLUMN
                bcc loadChar
                jsr _ScrollScreen
                lda #ROWSPERCOLUMN-1
                sta _cursorY

loadChar:       lda (adp1), y
                beq doneText

                cmp #$0a
                bne :+

                lda #0                              ; Back to the left edge
                sta _cursorX
                inc _cursorY                        ; Advance to the next line
                iny
                bne checkHWrap

:               sty temp
                ldx _cursorX
                ldy _cursorY
                jsr _DrawChar
                ldy temp
                inc _cursorX
                iny
                bne checkHWrap

doneText:       jsr CursorOn
                rts

demoText1:      .byte "  *** COMMODORE KIM-1 SHELL V0.1 ***", $0A, $0A
                .byte "   60K RAM SYSTEM.  49152 BYTES FREE.", $0A, $0A, $00
readyText:      .byte $0A,"READY.", $0A, 00

_Demo:          jsr _ClearScreen
                lda #0
                sta _cursorX
                sta _cursorY
                ldx #<demoText1
                ldy #>demoText1
                jsr _DrawText
                rts

_Ready:         ldx #<readyText
                ldy #>readyText
                jsr _DrawText
                rts


;-----------------------------------------------------------------------------------
; DrawLine    - Draws a line between two points
;-----------------------------------------------------------------------------------
; _x1cord (16-bit)
; _y1cord ( 8-bit)
; _x2cord (16-bit)
; _y2cord ( 8-bit)
;-----------------------------------------------------------------------------------
; Implements something like Bresenham's algorithm for drawing a line:
;-----------------------------------------------------------------------------------
; void DrawLine(int x0, int y0, int x1, int y1, byte val)
; {
;     int dx = abs(_x2cord - _x1cord), sx = _x1cord < _x2cord ? 1 : -1;
;     int dy = abs(_y2cord - _y1cord), sy = _y1cord < _y2cord ? 1 : -1;
;     int err = (dx > dy ? dx : -dy) / 2, e2;
;
;     while (1)
;     {
;         SETPIXEL(_x1cord, _y1cord, val);
;
;         if (_x1cord == _x2cord && _y1cord == _y2cord)
;             break;
;
;         e2 = err;
;
;         if (e2 > -dx)
;         {
;             err -= dy;
;             _x1cord += sx;
;         }
;         if (e2 < dy)
;         {
;             err += dx;
;             _y1cord += sy;
;         }
;     }
; }
;-----------------------------------------------------------------------------------

_DrawLine:      sta pixel

                ldx #$01                ; positive x-step for now
                stx sx

                ; Calculate dx = (x2cord - X1cord) and see if its positive or not

                lda _x2cord             ; Calculate dx = (x2cord - X1cord)
                sec
                sbc _x1cord
                sta dx
                lda _x2cord+1
                sbc _x1cord+1
                sta dx+1
                bpl calcdy              ; dx is positive (dx >= 0), so we're good

                ; dx was negative (dx < 0), so we set sx to -1 and get the absolute
                ;  value by subtracting the other direction

                ldx #$FF                ; negative x-step
                stx sx
                lda _x1cord             ; Calculate dx = (x2cord - X1cord)
                sec
                sbc _x2cord
                sta dx
                lda _x1cord+1
                sbc _x2cord+1
                sta dx+1

                ; Calculate dy = (y2cord - y1cord) and see if its positive or not

calcdy:         ldx #$01                ; positive y-step for now
                stx sy
                lda _y2cord
                sec
                sbc _y1cord
                sta dy
                bcs positivedy          ; If y2cord > y1cord, then dy is positive and we're good

                ; dy was negative (dy < 0), so we set sy to -1 and get the absolute value

                ldx #$FF                ; negative y-step
                stx sy
                lda _y1cord
                sec
                sbc _y2cord
                sta dy

                ; Now we have dx and dy, so we can calculate err, but first we need
                ;  to see if dx > dy or not

positivedy:     lda dx+1                ; Check if dx > dy (both are always positive now)
                bne dxgt                ; If MSB of dx is greater than zero, then dx > dy since dy is 8-bits
                lda dy
                cmp dx
                bcs dygte

dxgt:           lda dx                  ; We found dx>dy so set err = dx / 2
                sta err
                lda dx+1
                lsr
                sta err+1               ; err = dx/2
                ror err
                jmp loop

dygte:          lda #0                  ; we found dx <= dy so set err = -dy / 2
                sec
                sbc dy                  ; else err = -dy / 2
                ror
                ora #$80
                sta err
                lda #$FF
                sta err+1

                ; Now we have dx, dy, and err, so we can start drawing pixels

loop:           lda pixel
                beq clearpixel
                jsr _SetPixel           ; Plot the current _x1cord, _y1cord
                jmp next
clearpixel:     jsr _ClearPixel         ; Clear the current _x1cord, _y1cord

next:           lda _x1cord             ; if (_x1cord == _x2cord && _y1cord == _y2cord) then we rts
                cmp _x2cord
                bne noteq
                lda _y1cord
                cmp _y2cord
                bne noteq
                lda _x1cord+1
                cmp _x2cord+1
                bne noteq

                rts

noteq:          lda err                 ; e2 = err
                sta e2
                lda err+1
                sta e2+1

                ; Check the two update conditions for x and y, and update if needed

                lda e2                  ; if (e2 > -dx) is the same as if (e2 + dx > 0), so we test that because its easier
                clc                     ;    If its true then we dec err and inc _x1cord
                adc dx
                sta temp
                lda e2+1
                adc dx+1
                bmi doneupdatex         ; If result is negative, then e2 + dx < 0, so we don't dec err or inc _x1cord
                bne stepx                ; If MSB is non-zero, then e2 + dx > 0, so we DO dec err and inc _x1cord
                lda temp                ; If result is zero in MSB, then we check the LSB here
                beq doneupdatex         ;    If LSB is zero, then we don't dec err or inc _x1cord
                                        ; We already know e2 + dx > 0, so LSB can't be negative
stepx:          lda sx
                bmi decx
incxval:        inc _x1cord             ; _x1cord += 1 because sx == 1
                bne updatexerr
                inc _x1cord+1
                jmp updatexerr

decx:           lda _x1cord             ; _x1cord += 1 because sx == 1
                sec
                sbc #1
                sta _x1cord
                lda _x1cord+1
                sbc #0
                sta _x1cord+1

updatexerr:     lda err                 ; err -= dy
                sec
                sbc dy
                sta err
                lda err+1
                sbc #0
                sta err+1

doneupdatex:    lda e2+1                ; if (e2 < dy) then we inc err and inc _y1cord
                bmi updateerry          ; If MSB is negative, then e2 < dy, so we inc err and inc _y1cord
                bne noupdatey           ; If the MSB of e2 is set and positive, then we know e2 > dy, so we don't inc err or inc _y1cord
                lda e2
                sec
                sbc dy
                beq noupdatey           ; e2 - dy == 0 so we don't inc err or inc _y1cord
                bcs noupdatey           ; if e2 was large enough that carry never cleared, then e2 > dy do no update

updateerry:     lda err                 ; err += dx
                clc
                adc dx
                sta err
                lda err+1
                adc dx+1
                sta err+1

stepy:          lda _y1cord
                clc
                adc sy
                sta _y1cord

noupdatey:      jmp loop

_getch:         jsr     $1E5A            ; Get character using Monitor ROM call
                and     #$7F             ; Clear top bit
                cmp     #$0D             ; Check for '\r'
                bne     gotch            ; ...if CR character
                lda     #$0A             ; Replace with '\n'
gotch:          rts
