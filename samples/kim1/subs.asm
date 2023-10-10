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

SCREEN  = $A000

.segment "ZEROPAGE"
btpt:        .res 1

_x1cord:      .res 2
_x2cord:      .res 2
_y1cord:      .res 2
_y2cord:      .res 2

dest:
dest_lo:      .res 1
dest_hi:      .res 1

src:
src_lo:       .res 1
src_hi:       .res 1

adp1:
adp1_lo:      .res 1
adp1_hi:      .res 1

adp2:
adp2_lo:      .res 1
adp2_hi:      .res 1

.exportzp _x1cord
.exportzp _x2cord
.exportzp _y1cord
.exportzp _y2cord

.segment "DATA"

xval:           .res 2
yval:           .res 2 
err:            .res 2  
temp:           .res 2
temp2:          .res 2
x0:             .res 2
y0:             .res 2

.segment "CODE"

;-----------------------------------------------------------------------------------
; GetPixelAddress - Calculate the address of a pixel in the video memory
;-----------------------------------------------------------------------------------
; Based on MTU PIXADR code
;-----------------------------------------------------------------------------------
; x             - _x1cord (16-bit)
; y             - _y1cord (16-bit)   
; adp2          - address of pixel to set (16-bit)
;-----------------------------------------------------------------------------------

_GetPixelAddress:          
                lda 	_x1cord 	; compute bit address first
                sta 	adp1 		; also transfer _x1cord to adp1
                and 	#$07		; + which is simply the low 3 bits of x
                sta 	btpt

                lda 	_x1cord+1 	; finish transferring _x1cord to adp1
                sta 	adp1+1
                lsr 	adp1+1 		; double shift adp1 right 3 to get
                ror 	adp1 		; int(xcord/8 )
                lsr 	adp1+1
                ror 	adp1
                lsr 	adp1+1
                ror 	adp1
                lda 	#199 		; transfer (199-_y1cord) to adp2
                sec 			    ; and temporary storage
                sbc 	_y1cord
                sta 	adp2
                sta 	temp

                lda 	#0
                sbc 	_y1cord+1
                sta 	adp2+1
                sta 	temp+1
                asl 	adp2 		; compute 40*(199-_y1cord)
                rol 	adp2+1 		;  2*(199-_y1cord)
                asl 	adp2
                rol 	adp2+1 		;  4*(199-_y1cord)
                lda 	adp2 		;  add in temporary save of (199-_y1cord)
                clc 			    ;  to make 5*(199-_y1cord)
                adc 	temp
                sta 	adp2
                lda 	adp2+1
                adc 	temp+1
                sta 	adp2+1 		; 5*(199-_y1cord)
                asl 	adp2 		; 10*(199-_y1cord)
                rol 	adp2+1
                asl 	adp2 		; 20#(199-_y1cord)
                rol 	adp2+1
                asl 	adp2 		; 40*(199-_y1cord)
                rol 	adp2+1
                lda 	adp2 		; add in int(_x1cord/8) computed earlier
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
; x             - _x1cord (16-bit)
; y             - _y1cord (16-bit)   
;-----------------------------------------------------------------------------------
; Mask tables for individual pixel subroutines
;-----------------------------------------------------------------------------------

_SetPixel:	    jsr 	_GetPixelAddress
                ldy 	btpt 		; get bit number in y
                lda 	msktb1,y 	; get a byte with that bit =1, others =0
                ldy 	#0 		
                ora 	(adp1),y 	; combine the bit with the addressed vm
                sta 	(adp1),y 	; byte
                rts 			

;-----------------------------------------------------------------------------------
; ClearPixel - Clears a pixel in the video memory
;-----------------------------------------------------------------------------------
; x             - _x1cord (16-bit)
; y             - _y1cord (16-bit)   
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

;-----------------------------------------------------------------------------------
; ScrollScreen - Scrolls the entire video memory (and thus the screen) up one row
;-----------------------------------------------------------------------------------

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

;-----------------------------------------------------------------------------------
; DrawCircle    - Draws a circle in video memory of a given radius at a given coord
;-----------------------------------------------------------------------------------
; x             - _x1cord (16-bit)
; y             - _y1cord (16-bit)   
; radius        - _y2cord (16-bit)
;-----------------------------------------------------------------------------------
; Implements the midpoint circle algorithm without floating point or trig functions
;-----------------------------------------------------------------------------------

; void DrawCircle(int x0, int y0, int radius, byte val) 
; {
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
; }
       
_DrawCircle:    lda _x1cord                      ; x0 = _x1cord
                sta x0
                lda _x1cord+1
                sta x0+1
                lda _y1cord                      ; y0 = _y1cord
                sta y0
                lda _y1cord+1
                sta y0+1

                lda _y2cord                      ; x = radius
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

doCircle:                                       
                lda x0                          ; Draw the first of 8 symmetric quadrant copies
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
                
                lda temp                     ; temp will now be 2*(err-xval)+1
                asl
                sta temp
                lda temp+1
                rol
                sta temp+1
                inc temp                     
                bne :+
                inc temp+1
:               
                lda temp+1                   ; if (temp > 0) we'll dec xval
                bmi doneLoop                   ; less than zero, so no dec
                bne decxval                     ; if not zero, go ahead and dec

                lda temp                     ; MSb is zero so now check the LSB
                beq doneLoop                   ; both bytes are zero, so no dec

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

                lda #1                          ; temp2 = 1-(xval*2)
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
                
