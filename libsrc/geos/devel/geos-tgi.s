;
; Graphics driver for the 320x200x2 or 640x200x2 mode on GEOS 64/128
; Maciej 'YTM/Elysium' Witkowiak <ytm@elysium.pl>
; 28-31.12.2002

	.include 	"zeropage.inc"

      	.include 	"tgi-kernel.inc"
        .include        "tgi-mode.inc"
        .include        "tgi-error.inc"

	.include	"../inc/const.inc"
	.include	"../inc/jumptab.inc"
	.include	"../inc/geossym.inc"
	.include	"../inc/geossym2.inc"

        .macpack        generic

; ------------------------------------------------------------------------
; Constants

VDC_ADDR_REG	  = $D600		  ; VDC address
VDC_DATA_REG	  = $D601		  ; VDC data

VDC_DSP_HI	  = 12			  ; registers used
VDC_DSP_LO	  = 13
VDC_DATA_HI	  = 18
VDC_DATA_LO	  = 19
VDC_VSCROLL	  = 24
VDC_HSCROLL	  = 25
VDC_COLORS	  = 26
VDC_CSET	  = 28
VDC_COUNT	  = 30
VDC_DATA	  = 31

; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

.segment        "JUMPTABLE"

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

        .byte   $74, $67, $69           ; "tgi"
        .byte   TGI_API_VERSION         ; TGI API version number
xres:   .word   320                     ; X resolution
yres:   .word   200                     ; Y resolution
        .byte   2                       ; Number of drawing colors
pages:	.byte   1                       ; Number of screens available
        .byte   8                       ; System font X size
        .byte   8                       ; System font Y size
        .res    4, $00                  ; Reserved for future extensions

; Next comes the jump table. Currently all entries must be valid and may point
; to an RTS for test versions (function not implemented).

        .word   INSTALL
        .word   UNINSTALL
        .word   INIT
        .word   DONE
	.word	GETERROR
        .word   CONTROL
        .word   CLEAR
        .word   SETVIEWPAGE
        .word   SETDRAWPAGE
        .word   SETCOLOR
        .word   SETPALETTE
        .word   GETPALETTE
        .word   GETDEFPALETTE
        .word   SETPIXEL
        .word   GETPIXEL
        .word   LINE
        .word   BAR
        .word   CIRCLE
        .word   TEXTSTYLE
        .word   OUTTEXT

; ------------------------------------------------------------------------
; Data.

; Variables mapped to the zero page segment variables. Some of these are
; used for passing parameters to the driver.

X1              = ptr1
Y1              = ptr2
X2              = ptr3
Y2              = ptr4
RADIUS		= tmp1

ADDR		= tmp1
TEMP		= tmp3
TEMP2		= tmp4
TEMP3		= sreg
TEMP4		= sreg+1

; Circle stuff
XX		= ptr3		; (2)	CIRCLE
YY		= ptr4		; (2)	CIRCLE
MaxO		= sreg		; (overwritten by TEMP3+TEMP4, but restored from OG/OU anyway)
XS		= regsave	; (2)	CIRCLE
YS		= regsave+2	; (2)	CIRCLE

; Absolute variables used in the code

.bss

SCRBASE:	.res	1	; High byte of screen base (64k VDC only)

ERROR:  	.res	1     	; Error code
PALETTE:        .res    2       ; The current palette

BITMASK:        .res    1       ; $00 = clear, $01 = set pixels

OLDCOLOR:	.res	1	; colors before entering gfx mode

; Line routine stuff (combined with CIRCLE to save space)

OGora:		 .res	2
OUkos:		 .res	2
Y3:		 .res	2

; Text output stuff
TEXTMAGX:       .res    1
TEXTMAGY:       .res    1
TEXTDIR:        .res    1

; Constants and tables

.rodata

DEFPALETTE:     .byte   $00, $0f        ; White on black
PALETTESIZE     = * - DEFPALETTE

; color translation table (indexed by VIC color)
COLTRANS:	.byte $00, $0f, $08, $06, $0a, $04, $02, $0c
		.byte $0d, $0b, $09, $01, $0e, $05, $03, $07
		; colors BROWN and GRAY3 are wrong

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
;
; Must set an error code: NO
;

INSTALL:
	lda version		; if GEOS 1.0...
	and #$f0
	cmp #$10
	beq @L40
	lda c128Flag		; at least GEOS 2.0, but we're on C128?
	bpl @L40
        lda graphMode		; GEOS 2.0, C128, but is 80 column screen enabled?
        bmi @L80
@L40:	rts			; leave default values for 40 column screen

	; check for VDC version and update register $19 value

@L80:
	lda	#<640
	ldx	#>640
	sta	xres
	stx	xres+1

	; update number of available screens

	ldx	#VDC_CSET	; determine size of RAM...
	jsr	VDCReadReg
	sta	tmp1
	ora	#%00010000
	jsr	VDCWriteReg	; turn on 64k

	jsr	settestadr1	; save original value of test byte
	jsr	VDCReadByte
	sta	tmp2

	lda	#$55		; write $55 here
	ldy	#ptr1
	jsr	test64k		; read it here and there
	lda	#$aa		; write $aa here
	ldy	#ptr2
	jsr	test64k		; read it here and there

	jsr	settestadr1
	lda	tmp2
	jsr	VDCWriteByte	; restore original value of test byte

	lda	ptr1		; do bytes match?
	cmp	ptr1+1
	bne	@have64k
	lda	ptr2
	cmp	ptr2+1
	bne	@have64k

	ldx	#VDC_CSET
	lda	tmp1
	jsr	VDCWriteReg	; restore 16/64k flag
	jmp	@endok		; and leave default values for 16k

@have64k:
	lda	#4
	sta	pages
@endok:
	lda     #0
	sta	SCRBASE		; draw page 0 as default
        rts

test64k:
	sta	tmp1
	sty	ptr3
	lda	#0
	sta	ptr3+1
	jsr	settestadr1
	lda	tmp1
	jsr	VDCWriteByte		; write $55
	jsr	settestadr1
	jsr	VDCReadByte		; read here
	pha
	jsr	settestadr2
	jsr	VDCReadByte		; and there
	ldy	#1
	sta	(ptr3),y
	pla
	dey
	sta	(ptr3),y
	rts

settestadr1:
	ldy	#$02			; test page 2 (here)
	.byte	$2c
settestadr2:
	ldy	#$42			; or page 64+2 (there)
	lda	#0
	jmp	VDCSetSourceAddr

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory. May
; clean up anything done by INSTALL but is probably empty most of the time.
;
; Must set an error code: NO
;

UNINSTALL:
        rts


; ------------------------------------------------------------------------
; INIT: Changes an already installed device from text mode to graphics
; mode.
; Note that INIT/DONE may be called multiple times while the driver
; is loaded, while INSTALL is only called once, so any code that is needed
; to initializes variables and so on must go here. Setting palette and
; clearing the screen is not needed because this is called by the graphics
; kernel later.
; The graphics kernel will never call INIT when a graphics mode is already
; active, so there is no need to protect against that.
;
; Must set an error code: YES
;

INIT:
	ldx #$01
	stx BITMASK		; solid black as pattern
	lda #1
	jsr SetPattern
	lda #ST_WR_FORE		; write only on foreground
	sta dispBufferOn

        lda graphMode
        bmi @L80

; Remember current color value (40 columns)
	lda screencolors
	sta OLDCOLOR
	jmp @L99

; Remember current color value (80 columns)
@L80:	lda scr80colors
	sta OLDCOLOR
@L99:	lda #0
	jsr SETVIEWPAGE		; switch into viewpage 0

; Done, reset the error code

	lda     #TGI_ERR_OK
	sta     ERROR
        rts

; ------------------------------------------------------------------------
; DONE: Will be called to switch the graphics device back into text mode.
; The graphics kernel will never call DONE when no graphics mode is active,
; so there is no need to protect against that.
;
; Must set an error code: NO
;

DONE:
	lda #0
	jsr SETVIEWPAGE		; switch into viewpage 0

	lda graphMode
	bmi @L80

	lda OLDCOLOR
	sta screencolors	; restore color for 40 columns
	ldx #0
@L1:	sta COLOR_MATRIX,x
	sta COLOR_MATRIX+$0100,x
	sta COLOR_MATRIX+$0200,x
	sta COLOR_MATRIX+1000-256,x
	inx
	bne @L1
	rts

@L80:	lda OLDCOLOR		; restore color for 80 columns
	ldx #VDC_COLORS
	jmp VDCWriteReg

; ------------------------------------------------------------------------
; GETERROR: Return the error code in A and clear it.

GETERROR:
       	ldx	#TGI_ERR_OK
	lda	ERROR
	stx	ERROR
        rts

; ------------------------------------------------------------------------
; CONTROL: Platform/driver specific entry point.
;
; Must set an error code: YES
;

CONTROL:
	lda	#TGI_ERR_INV_FUNC
	sta	ERROR
        rts

; ------------------------------------------------------------------------
; CLEAR: Clears the screen.
;
; Must set an error code: NO
;

CLEAR:
	    lda curPattern
	    pha
	    lda #0
	    jsr SetPattern
	    ldx #0
	    stx r3L
	    stx r3H
	    stx r2L
	    lda #199
	    sta r2H
	    lda graphMode
	    bpl @L40
	    lda #>639			; 80 columns
	    ldx #<639
	    bne @L99
@L40:	    lda #>319			; 40 columns
	    ldx #<319
@L99:	    sta r4H
	    stx r4L
	    jsr Rectangle
	    pla
	    sta curPattern
	    rts

; ------------------------------------------------------------------------
; SETVIEWPAGE: Set the visible page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;

SETVIEWPAGE:
	ldx graphMode
	bmi @L80
	rts
@L80:	clc
	ror
	ror
	ror
	ldx	#VDC_DSP_HI
	jmp	VDCWriteReg

; ------------------------------------------------------------------------
; SETDRAWPAGE: Set the drawable page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;

SETDRAWPAGE:
	ldx graphMode
	bmi @L80
	rts
@L80:	clc
	ror
	ror
	ror
	sta	SCRBASE
	rts

; ------------------------------------------------------------------------
; SETCOLOR: Set the drawing color (in A). The new color is already checked
; to be in a valid range (0..maxcolor-1).
;
; Must set an error code: NO (will only be called if color ok)
;

SETCOLOR:
        tax
        beq     @L1
        lda	#1
@L1:    sta     BITMASK
	jmp	SetPattern	; need to have either 0 or 1

; ------------------------------------------------------------------------
; SETPALETTE: Set the palette (not available with all drivers/hardware).
; A pointer to the palette is passed in ptr1. Must set an error if palettes
; are not supported
;
; Must set an error code: YES
;

SETPALETTE:
	jsr	GETERROR	; clear error (if any)

        ldy     #PALETTESIZE - 1
@L1:    lda     (ptr1),y        ; Copy the palette
        and     #$0F            ; Make a valid color
        sta     PALETTE,y
        dey
        bpl     @L1

; Put colors from palette into screen

	lda	graphMode
	bmi	@L80

	lda	PALETTE+1	; foreground
	asl	a
	asl	a
	asl	a
	asl	a
	ora	PALETTE		; background
	ldx	#0
@L2:	sta	COLOR_MATRIX,x
	sta	COLOR_MATRIX+$0100,x
	sta	COLOR_MATRIX+$0200,x
	sta	COLOR_MATRIX+1000-256,x
	inx
	bne	@L2
	rts

@L80:   ldy     PALETTE+1       ; Foreground color
	lda	COLTRANS,y
        asl     a
        asl     a
        asl     a
        asl     a
        ldy     PALETTE         ; Background color
	ora	COLTRANS,y

	ldx	#VDC_COLORS
	jmp	VDCWriteReg

; ------------------------------------------------------------------------
; GETPALETTE: Return the current palette in A/X. Even drivers that cannot
; set the palette should return the default palette here, so there's no
; way for this function to fail.
;
; Must set an error code: NO
;

GETPALETTE:
        lda     #<PALETTE
        ldx     #>PALETTE
        rts

; ------------------------------------------------------------------------
; GETDEFPALETTE: Return the default palette for the driver in A/X. All
; drivers should return something reasonable here, even drivers that don't
; support palettes, otherwise the caller has no way to determine the colors
; of the (not changeable) palette.
;
; Must set an error code: NO (all drivers must have a default palette)
;

GETDEFPALETTE:
        lda     #<DEFPALETTE
        ldx     #>DEFPALETTE
        rts

; ------------------------------------------------------------------------
; SETPIXEL: Draw one pixel at X1/Y1 = ptr1/ptr2 with the current drawing
; color. The coordinates passed to this function are never outside the
; visible screen area, so there is no need for clipping inside this function.
;
; Must set an error code: NO
;

SETPIXELCLIP:
	lda	Y1+1
	bmi	@finito		; y<0
	lda	X1+1
	bmi	@finito		; x<0
	lda	X1
	ldx	X1+1
	sta	ADDR
	stx	ADDR+1
	ldx	#ADDR
	lda	xres
	ldy	xres+1
	jsr	icmp		; ( x < xres ) ...
	bcs	@finito
	lda	Y1
	ldx	Y1+1
	sta	ADDR
	stx	ADDR+1
	ldx	#ADDR
	lda	yres
	ldy	yres+1
	jsr	icmp		; ... && ( y < yres )
	bcc	SETPIXEL
@finito:rts

SETPIXEL:
	lda	X1
	ldx	X1+1
	ldy	Y1
	sta	r3L
	stx	r3H
	sty	r11L
	sec
	lda	BITMASK		; set or clear C flag
	bne	@L1
	clc
@L1:	lda	#0
	jmp	DrawPoint

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.


GETPIXEL:
	lda	X1
	ldx	X1+1
	ldy	Y1
	sta	r3L
	stx	r3H
	sty	r11L
	jsr	TestPoint
	ldx	#0
	bcc	@L1
	inx
@L1:	txa
	ldx	#0
	rts

; ------------------------------------------------------------------------
; LINE: Draw a line from X1/Y1 to X2/Y2, where X1/Y1 = ptr1/ptr2 and
; X2/Y2 = ptr3/ptr4 using the current drawing color.
;
; Must set an error code: NO
;

LINE:
	lda	X1
	ldx	X1+1
	ldy	Y1
	sta	r3L
	stx	r3H
	sty	r11L
	lda	X2
	ldx	X2+1
	ldy	Y2
	sta	r4L
	stx	r4H
	sty	r11H
	sec
	lda	BITMASK		; set or clear C flag
	bne	@L1
	clc
@L1:	lda	#0
	jmp	DrawLine

; ------------------------------------------------------------------------
; BAR: Draw a filled rectangle with the corners X1/Y1, X2/Y2, where
; X1/Y1 = ptr1/ptr2 and X2/Y2 = ptr3/ptr4 using the current drawing color.
; Contrary to most other functions, the graphics kernel will sort and clip
; the coordinates before calling the driver, so on entry the following
; conditions are valid:
;       X1 <= X2
;       Y1 <= Y2
;       (X1 >= 0) && (X1 < XRES)
;       (X2 >= 0) && (X2 < XRES)
;       (Y1 >= 0) && (Y1 < YRES)
;       (Y2 >= 0) && (Y2 < YRES)
;
; Must set an error code: NO
;

BAR:
	lda	X1
	ldx	X1+1
	ldy	Y1
	sta	r3L
	stx	r3H
	sty	r2L
	lda	X2
	ldx	X2+1
	ldy	Y2
	sta	r4L
	stx	r4H
	sty	r2H
	jmp	Rectangle

; ------------------------------------------------------------------------
; CIRCLE: Draw a circle around the center X1/Y1 (= ptr1/ptr2) with the
; radius in tmp1 and the current drawing color.
;
; Must set an error code: NO
;

CIRCLE:
	lda     RADIUS
        bne     @L1
        jmp     SETPIXELCLIP    ; Plot as a point

@L1:	sta	XX
	; x = r;
	lda	#0
	sta	XX+1
  	sta	YY
	sta	YY+1
	sta	MaxO
	sta	MaxO+1
	; y =0; mo=0;
	lda	X1
	ldx	X1+1
	sta	XS
	stx	XS+1
	lda	Y1
	ldx	Y1+1
	sta	YS
	stx	YS+1		; XS/YS to remember the center

	; while (y<x) {
@L013B:	ldx	#YY
	lda	XX
	ldy	XX+1
	jsr	icmp
	bcc	@L12
	rts
@L12:	; plot points in 8 slices...
	lda	XS
	clc
	adc	XX
	sta	X1
	lda	XS+1
	adc	XX+1
	sta	X1+1		; x1 = xs+x
	lda	YS
	clc
	adc	YY
	sta	Y1
	pha
	lda	YS+1
	adc	YY+1
	sta	Y1+1		; (stack)=ys+y, y1=(stack)
	pha
	jsr	SETPIXELCLIP	; plot(xs+x,ys+y)
	lda	YS
	sec
	sbc	YY
	sta	Y1
	sta	Y3
	lda	YS+1
	sbc	YY+1
	sta	Y1+1		; y3 = y1 = ys-y
	sta	Y3+1
	jsr	SETPIXELCLIP	; plot(xs+x,ys-y)
	pla
	sta	Y1+1
	pla
	sta	Y1		; y1 = ys+y
	lda	XS
	sec
	sbc	XX
	sta	X1
	lda	XS+1
	sbc	XX+1
	sta	X1+1
	jsr	SETPIXELCLIP	; plot (xs-x,ys+y)
	lda	Y3
	sta	Y1
	lda	Y3+1
	sta	Y1+1
	jsr	SETPIXELCLIP	; plot (xs-x,ys-y)

	lda	XS
	clc
	adc	YY
	sta	X1
	lda	XS+1
	adc	YY+1
	sta	X1+1		; x1 = xs+y
	lda	YS
	clc
	adc	XX
	sta	Y1
	pha
	lda	YS+1
	adc	XX+1
	sta	Y1+1		; (stack)=ys+x, y1=(stack)
	pha
	jsr	SETPIXELCLIP	; plot(xs+y,ys+x)
	lda	YS
	sec
	sbc	XX
	sta	Y1
	sta	Y3
	lda	YS+1
	sbc	XX+1
	sta	Y1+1		; y3 = y1 = ys-x
	sta	Y3+1
	jsr	SETPIXELCLIP	; plot(xs+y,ys-x)
	pla
	sta	Y1+1
	pla
	sta	Y1		; y1 = ys+x(stack)
	lda	XS
	sec
	sbc	YY
	sta	X1
	lda	XS+1
	sbc	YY+1
	sta	X1+1
	jsr	SETPIXELCLIP	; plot (xs-y,ys+x)
	lda	Y3
	sta	Y1
	lda	Y3+1
	sta	Y1+1
	jsr	SETPIXELCLIP	; plot (xs-y,ys-x)

	; og = mo+y+y+1
	lda	MaxO
	ldx	MaxO+1
	clc
	adc	YY
	tay
	txa
	adc	YY+1
	tax
	tya
	clc
	adc	YY
	tay
	txa
	adc	YY+1
	tax
	tya
	clc
	adc	#1
	bcc	@L0143
	inx
@L0143:	sta	OGora
	stx	OGora+1
	; ou = og-x-x+1
	sec
	sbc	XX
	tay
	txa
	sbc	XX+1
	tax
	tya
	sec
	sbc	XX
	tay
	txa
	sbc	XX+1
	tax
	tya
	clc
	adc	#1
	bcc	@L0146
	inx
@L0146:	sta	OUkos
	stx	OUkos+1
	; ++y
	inc	YY
	bne	@L0148
	inc	YY+1
@L0148:	; if (abs(ou)<abs(og))
	lda	OUkos
	ldy	OUkos+1
	jsr	abs
	sta	TEMP3
	sty	TEMP4
	lda	OGora
	ldy	OGora+1
	jsr	abs
	ldx	#TEMP3
	jsr	icmp
	bpl	@L0149
	; { --x;
	sec
	lda	XX
	sbc	#1
	sta	XX
	bcs	@L014E
	dec	XX+1
@L014E:	; mo = ou; }
	lda	OUkos
	ldx	OUkos+1
	jmp	@L014G
	; else { mo = og }
@L0149:	lda	OGora
	ldx	OGora+1
@L014G:	sta	MaxO
	stx	MaxO+1
	; }
        jmp	@L013B

; ------------------------------------------------------------------------
; TEXTSTYLE: Set the style used when calling OUTTEXT. Text scaling in X and Y
; direction is passend in X/Y, the text direction is passed in A.
;
; Must set an error code: NO
;

TEXTSTYLE:
        stx     TEXTMAGX
        sty     TEXTMAGY
        sta     TEXTDIR
        rts


; ------------------------------------------------------------------------
; OUTTEXT: Output text at X/Y = ptr1/ptr2 using the current color and the
; current text style. The text to output is given as a zero terminated
; string with address in ptr3.
;
; Must set an error code: NO
;

OUTTEXT:
	lda	TEXTDIR
;	cmp	#TGI_TEXT_HORIZONTAL	; this is equal 0
	bne	@vertical

	lda	X1		; horizontal text output
	ldx	X1+1
	ldy	Y1
	sta	r11L
	stx	r11H
	sty	r1H
	lda	ptr3
	ldx	ptr3+1
	sta	r0L
	stx	r0H
	jmp	PutString

@vertical:
	lda	X1		; vertical text output
	ldx	X1+1
	ldy	Y1
	sta 	r11L
	stx	r11H
	sty	r1H
	ldy	#0
	lda	(ptr3),y
	beq	@end
	jsr	PutChar
	inc	ptr3
	bne	@L1
	inc	ptr3+1
@L1:	lda	Y1
	clc
	adc	#8
	sta	Y1
	bne	@vertical
@end:	rts

;-------------
; copies of some runtime routines

abs:
	; a/y := abs(a/y)
	dey
	iny
	bpl	@L1
	; negay
	clc
	eor	#$ff
	adc	#1
	pha
	tya
	eor	#$ff
	adc	#0
	tay
	pla
@L1:	rts

icmp:
	; compare a/y to zp,x
	sta	TEMP		; TEMP/TEMP2 - arg2
	sty	TEMP2
	lda	$0,x
	pha
	lda	$1,x
	tay
	pla
	tax
	tya			; x/a - arg1 (a=high)

	sec
	sbc	TEMP2
	bne	@L4
	cpx	TEMP
	beq	@L3
	adc	#$ff
	ora	#$01
@L3:	rts
@L4:	bvc	@L3
	eor	#$ff
	ora	#$01
	rts

;-------------
; VDC helpers

VDCSetSourceAddr:
	pha
	tya
	ldx	#VDC_DATA_HI
	jsr	VDCWriteReg
	pla
	ldx	#VDC_DATA_LO
	bne	VDCWriteReg

VDCReadByte:
	ldx	#VDC_DATA
VDCReadReg:
	stx	VDC_ADDR_REG
@L0:	bit	VDC_ADDR_REG
	bpl	@L0
	lda	VDC_DATA_REG
	rts

VDCWriteByte:
	ldx	#VDC_DATA
VDCWriteReg:
	stx	VDC_ADDR_REG
@L0:	bit	VDC_ADDR_REG
	bpl	@L0
	sta	VDC_DATA_REG
	rts

