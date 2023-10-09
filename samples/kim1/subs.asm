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

.segment "ZEROPAGE"
temp:        .res 1
btpt:        .res 1

dest    = $02
dest_lo = dest
dest_hi = dest+1

src    = $04
src_lo = src
src_hi = src+1

adp1    = $06
adp1_lo = adp1
adp1_hi = adp1+1

adp2    = $08
adp2_lo = adp2
adp2_hi = adp2+1

x1cord  = $0A
x1cord_lo = x1cord
x1cord_hi = x1cord+1

y1cord  = $0C
y1cord_lo = y1cord
y1cord_hi = y1cord+1


.segment "CODE"

;-----------------------------------------------------------------------------------
; GetPixelAddress - Calculate the address of a pixel in the video memory
;-----------------------------------------------------------------------------------
; Based on MTU PIXADR code
;-----------------------------------------------------------------------------------
; x             - x1cord (16-bit)
; y             - y1cord (16-bit)   
; adp2          - address of pixel to set (16-bit)
;-----------------------------------------------------------------------------------

_GetPixelAddress:          
                lda 	x1cord 		; compute bit address first
                sta 	adp1 		; also transfer x1cord to adp1
                and 	#$07		; + which is simply the low 3 bits of x
                sta 	btpt

                lda 	x1cord+1 	; finish transferring x1cord to adp1
                sta 	adp1+1
                lsr 	adp1+1 		; double shift adp1 right 3 to get
                ror 	adp1 		; int(xcord/8 )
                lsr 	adp1+1
                ror 	adp1
                lsr 	adp1+1
                ror 	adp1
                lda 	#199 		; transfer (199-y1cord) to adp2
                sec 			    ; and temporary storage
                sbc 	y1cord
                sta 	adp2
                sta 	temp

                lda 	#0
                sbc 	y1cord+1
                sta 	adp2+1
                sta 	temp+1
                asl 	adp2 		; compute 40*(199-y1cord)
                rol 	adp2+1 		;  2*(199-y1cord)
                asl 	adp2
                rol 	adp2+1 		;  4*(199-y1cord)
                lda 	adp2 		;  add in temporary save of (199-y1cord)
                clc 			    ;  to make 5*(199-y1cord)
                adc 	temp
                sta 	adp2
                lda 	adp2+1
                adc 	temp+1
                sta 	adp2+1 		; 5*(199-y1cord)
                asl 	adp2 		; 10*(199-y1cord)
                rol 	adp2+1
                asl 	adp2 		; 20#(199-y1cord)
                rol 	adp2+1
                asl 	adp2 		; 40*(199-y1cord)
                rol 	adp2+1
                lda 	adp2 		; add in int(x1cord/8) computed earlier
                clc
                adc 	adp1
                sta 	adp1
                lda 	adp2+1
                adc 	adp1+1
                adc 	#>SCREEN    ; add in base vidscreen address
                sta 	adp1+1 		; final result
                rts 			    ; return

;-----------------------------------------------------------------------------------
; Mask tables for individual pixel subroutines
;
; MSKTB1 is a table of 1 bits corresponding to bit numbers
; MSKTB2 is a table of 0 bits corresponding to bit numbers
;-----------------------------------------------------------------------------------

msktb1:	 	    .byte 	$80,$40,$20,$10,$08,$04,$02,$01
msktb2:	  	    .byte 	$7F,$BF,$DF,$EF,$F7,$FB,$FD,$FE

;-----------------------------------------------------------------------------------
; SetPixel - Set a pixel in the video memory
;-----------------------------------------------------------------------------------
; x             - x1cord (16-bit)
; y             - y1cord (16-bit)   
;-----------------------------------------------------------------------------------
; Mask tables for individual pixel subroutines
;-----------------------------------------------------------------------------------

_SetPixel:	 	jsr 	_GetPixelAddress
                ldy 	btpt 		; get bit number in y
                lda 	msktb1,y 	; get a byte with that bit =1, others =0
                ldy 	#0 		
                ora 	(adp1),y 	; combine the bit with the addressed vm
                sta 	(adp1),y 	; byte
                rts 			

;-----------------------------------------------------------------------------------
; ClearPixel - Clears a pixel in the video memory
;-----------------------------------------------------------------------------------
; x             - x1cord (16-bit)
; y             - y1cord (16-bit)   
;-----------------------------------------------------------------------------------

_ClearPixel:	jsr 	_GetPixelAddress
                ldy 	btpt 		; get bit number in y
                lda 	msktb2,y 	; get a byte with that bit =0, others =1
                ldy 	#0 		
                and 	(adp1),y 	; remove the bit from the addressed vm
                sta 	(adp1),y 	; byte
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




