;
; Graphics driver for the 40x40x16 mode on the Apple II
;
; Stefan Haubenthal <polluks@sdf.lonestar.org>
; Based on Maciej Witkowiak's line and circle routine
;

	.include	"zeropage.inc"

	.include	"tgi-kernel.inc"
	.include	"tgi-mode.inc"
	.include	"tgi-error.inc"
	.include	"apple2.inc"

	.macpack	generic

; ------------------------------------------------------------------------
; Constants

H2		= $2C
PLOT		= $F800
HLINE		= $F819
CLRTOP		= $F836
SETCOL		= $F864
SCRN		= $F871
SETGR		= $FB40

; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

.segment	"JUMPTABLE"

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

	.byte	$74, $67, $69		; "tgi"
	.byte	TGI_API_VERSION		; TGI API version number
xres:	.word	40			; X resolution
yres:	.word	40			; Y resolution
	.byte	16			; Number of drawing colors
	.byte	1			; Number of screens available
	.byte	8			; System font X size
	.byte	8			; System font Y size
	.res	4, $00			; Reserved for future extensions

; Next comes the jump table. Currently all entries must be valid and may point
; to an RTS for test versions (function not implemented).

	.word	INSTALL
	.word	UNINSTALL
	.word	INIT
	.word	DONE
	.word	GETERROR
	.word	CONTROL
	.word	CLEAR
	.word	SETVIEWPAGE
	.word	SETDRAWPAGE
	.word	SETCOLOR
	.word	SETPALETTE
	.word	GETPALETTE
	.word	GETDEFPALETTE
	.word	SETPIXEL
	.word	GETPIXEL
	.word	LINE
	.word	BAR
	.word	CIRCLE
	.word	TEXTSTYLE
	.word	OUTTEXT

; ------------------------------------------------------------------------
; Data.

; Variables mapped to the zero page segment variables. Some of these are
; used for passing parameters to the driver.

X1		= ptr1
Y1		= ptr2
X2		= ptr3
Y2		= ptr4
RADIUS		= tmp1

ADDR		= tmp1
TEMP		= tmp3
TEMP2		= tmp4
TEMP3		= sreg
TEMP4		= sreg+1

; Line routine stuff (must be on zpage)
PB 		= ptr3 		; (2)	LINE
UB		= ptr4 		; (2)	LINE
ERR		= regsave 	; (2)	LINE
NX		= regsave+2 	; (2)	LINE
; Circle stuff
XX		= ptr3		; (2)	CIRCLE
YY		= ptr4		; (2)	CIRCLE
MaxO		= sreg		; (overwritten by TEMP3+TEMP4, but restored from OG/OU anyway)
XS		= regsave	; (2)	CIRCLE
YS		= regsave+2	; (2)	CIRCLE

; Absolute variables used in the code

.bss

ERROR:		.res	1	; Error code

; Line routine stuff (combined with CIRCLE to save space)

OGora:
COUNT:		.res	2
OUkos:
NY:		.res	2
Y3:
DX:		.res	1
DY:		.res	1
AY:		.res	1

; Constants and tables

.rodata

DEFPALETTE:	.byte	$00, $0F, $01, $0E, $03, $04, $02, $0D
		.byte	$09, $08, $0B, $05, $0A, $0C, $06, $07

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
;
; Must set an error code: NO
;

INSTALL:


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

; Switch into graphics mode

	jsr	SETGR

; Done, reset the error code

	lda	#TGI_ERR_OK
	sta	ERROR
	rts

; ------------------------------------------------------------------------
; DONE: Will be called to switch the graphics device back into text mode.
; The graphics kernel will never call DONE when no graphics mode is active,
; so there is no need to protect against that.
;
; Must set an error code: NO
;

DONE		= TEXT

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

CLEAR		= CLRTOP

; ------------------------------------------------------------------------
; SETVIEWPAGE: Set the visible page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;

SETVIEWPAGE:

; ------------------------------------------------------------------------
; SETDRAWPAGE: Set the drawable page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;

SETDRAWPAGE:
	rts

; ------------------------------------------------------------------------
; SETCOLOR: Set the drawing color (in A). The new color is already checked
; to be in a valid range (0..maxcolor-1).
;
; Must set an error code: NO (will only be called if color ok)
;

SETCOLOR	= SETCOL

; ------------------------------------------------------------------------
; SETPALETTE: Set the palette (not available with all drivers/hardware).
; A pointer to the palette is passed in ptr1. Must set an error if palettes
; are not supported
;
; Must set an error code: YES
;

SETPALETTE:
	lda	#TGI_ERR_INV_FUNC
	sta	ERROR
	rts

; ------------------------------------------------------------------------
; GETPALETTE: Return the current palette in A/X. Even drivers that cannot
; set the palette should return the default palette here, so there's no
; way for this function to fail.
;
; Must set an error code: NO
;

GETPALETTE:

; ------------------------------------------------------------------------
; GETDEFPALETTE: Return the default palette for the driver in A/X. All
; drivers should return something reasonable here, even drivers that don't
; support palettes, otherwise the caller has no way to determine the colors
; of the (not changeable) palette.
;
; Must set an error code: NO (all drivers must have a default palette)
;

GETDEFPALETTE:
	lda	#<DEFPALETTE
	ldx	#>DEFPALETTE
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
	ldy	Y1
	jmp	PLOT

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.


GETPIXEL:
	lda	X1
	ldy	Y1
	jsr	SCRN
	ldx	#$00
	rts

; ------------------------------------------------------------------------
; LINE: Draw a line from X1/Y1 to X2/Y2, where X1/Y1 = ptr1/ptr2 and
; X2/Y2 = ptr3/ptr4 using the current drawing color.
;
; Must set an error code: NO
;

LINE:
	; nx = abs(x2 - x1)
	lda	X2
	sub	X1
	sta	NX
	lda	X2+1
	sbc	X1+1
	tay
	lda	NX
	jsr	abs
	sta	NX
	sty	NX+1
	; ny = abs(y2 - y1)
	lda	Y2
	sub	Y1
	sta	NY
	lda	Y2+1
	sbc	Y1+1
	tay
	lda	NY
	jsr	abs
	sta	NY
	sty	NY+1
	; if (x2>x1)
	ldx	#X2
	lda	X1
	ldy	X1+1
	jsr	icmp
	bcc	@L0243
	beq	@L0243
	; dx = 1;
	lda	#$01
	bne	@L0244
	; else
	; dx = -1;
@L0243:	lda	#$FF
@L0244:	sta	DX
	; if (y2>y1)
	ldx	#Y2
	lda	Y1
	ldy	Y1+1
	jsr	icmp
	bcc	@L024A
	beq	@L024A
	; dy = 1;
	lda	#$01
	bne	@L024B
	; else
	; dy = -1;
@L024A:	lda	#$FF
@L024B:	sta	DY
	; err = ay = 0;
	lda	#$00
	sta	ERR
	sta	ERR+1
	sta	AY

	; if (nx<ny) {
	ldx	#NX
	lda	NY
	ldy	NY+1
	jsr	icmp
	bcs	@L0255
	;  nx <-> ny
	lda	NX
	ldx	NY
	sta	NY
	stx	NX
	lda	NX+1
	ldx	NY+1
	sta	NY+1
	stx	NX+1
	; ay = dx
	lda	DX
	sta	AY
	; dx = dy = 0;
	lda	#$00
	sta	DX
	sta	DY
	; ny = - ny;
@L0255:	lda	NY
	ldy	NY+1
	jsr	neg
	sta	NY
	sty	NY+1
	; for (count=nx;count>0;--count) {
	lda	NX
	ldx	NX+1
	sta	COUNT
	stx	COUNT+1
@L0166:	lda	COUNT		; count>0
	ora	COUNT+1
	bne	@L0167
	rts
	;    setpixel(X1,Y1)
@L0167:	jsr	SETPIXELCLIP
	;    pb = err + ny
	lda	ERR
	add	NY
	sta	PB
	lda	ERR+1
	adc	NY+1
	sta	PB+1
	tax
	;    ub = pb + nx
	lda	PB
	add	NX
	sta	UB
	txa
	adc	NX+1
	sta	UB+1
	;    x1 = x1 + dx
	ldx	#$00
	lda	DX
	bpl	@L027B
	dex
@L027B:	add	X1
	sta	X1
	txa
	adc	X1+1
	sta	X1+1
	;   y1 = y1 + ay
	ldx	#$00
	lda	AY
	bpl	@L027E
	dex
@L027E:	add	Y1
	sta	Y1
	txa
	adc	Y1+1
	sta	Y1+1
	; if (abs(pb)<abs(ub)) {
	lda	PB
	ldy	PB+1
	jsr	abs
	sta	TEMP3
	sty	TEMP4
	lda	UB
	ldy	UB+1
	jsr	abs
	ldx	#TEMP3
	jsr	icmp
	bpl	@L027F
	;   err = pb
	lda	PB
	ldx	PB+1
	jmp	@L0312
	; } else { x1 = x1 + ay
@L027F:
	ldx	#$00
	lda	AY
	bpl	@L0288
	dex
@L0288:	add	X1
	sta	X1
	txa
	adc	X1+1
	sta	X1+1
	;	y1 = y1 + dy
	ldx	#$00
	lda	DY
	bpl	@L028B
	dex
@L028B:	add	Y1
	sta	Y1
	txa
	adc	Y1+1
	sta	Y1+1
	;	err = ub }
	lda	UB
	ldx	UB+1
@L0312:
	sta	ERR
	stx	ERR+1
	; } (--count)
	sec
	lda	COUNT
	sbc	#$01
	sta	COUNT
	bcc	@L0260
	jmp	@L0166
@L0260:	dec	COUNT+1
	jmp	@L0166

; ------------------------------------------------------------------------
; BAR: Draw a filled rectangle with the corners X1/Y1, X2/Y2, where
; X1/Y1 = ptr1/ptr2 and X2/Y2 = ptr3/ptr4 using the current drawing color.
; Contrary to most other functions, the graphics kernel will sort and clip
; the coordinates before calling the driver, so on entry the following
; conditions are valid:
;	X1 <= X2
;	Y1 <= Y2
;	(X1 >= 0) && (X1 < XRES)
;	(X2 >= 0) && (X2 < XRES)
;	(Y1 >= 0) && (Y1 < YRES)
;	(Y2 >= 0) && (Y2 < YRES)
;
; Must set an error code: NO
;

BAR:
	inc	Y2
	ldx	X2
	stx	H2
@L1:	ldy	X1
	lda	Y1
	jsr	HLINE
	inc	Y1
	lda	Y2
	cmp	Y1
	bne	@L1
	rts

; ------------------------------------------------------------------------
; CIRCLE: Draw a circle around the center X1/Y1 (= ptr1/ptr2) with the
; radius in tmp1 and the current drawing color.
;
; Must set an error code: NO
;

CIRCLE:
	lda	RADIUS
	bne	@L1
	jmp	SETPIXELCLIP	; Plot as a point

@L1:	sta	XX
	; x = r;
	lda	#$00
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
@L013B: ldx	#YY
	lda	XX
	ldy	XX+1
	jsr	icmp
	bcc	@L12
	rts
@L12:	; plot points in 8 slices...
	lda	XS
	add	XX
	sta	X1
	lda	XS+1
	adc	XX+1
	sta	X1+1		; x1 = xs+x
	lda	YS
	add	YY
	sta	Y1
	pha
	lda	YS+1
	adc	YY+1
	sta	Y1+1		; (stack)=ys+y, y1=(stack)
	pha
	jsr	SETPIXELCLIP	; plot(xs+x,ys+y)
	lda	YS
	sub	YY
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
	sub	XX
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
	add	YY
	sta	X1
	lda	XS+1
	adc	YY+1
	sta	X1+1		; x1 = xs+y
	lda	YS
	add	XX
	sta	Y1
	pha
	lda	YS+1
	adc	XX+1
	sta	Y1+1		; (stack)=ys+x, y1=(stack)
	pha
	jsr	SETPIXELCLIP	; plot(xs+y,ys+x)
	lda	YS
	sub	XX
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
	sub	YY
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
	add	YY
	tay
	txa
	adc	YY+1
	tax
	tya
	add	YY
	tay
	txa
	adc	YY+1
	tax
	tya
	add	#$01
	bcc	@L0143
	inx
@L0143: sta	OGora
	stx	OGora+1
	; ou = og-x-x+1
	sub	XX
	tay
	txa
	sbc	XX+1
	tax
	tya
	sub	XX
	tay
	txa
	sbc	XX+1
	tax
	tya
	add	#$01
	bcc	@L0146
	inx
@L0146: sta	OUkos
	stx	OUkos+1
	; ++y
	inc	YY
	bne	@L0148
	inc	YY+1
@L0148: ; if (abs(ou)<abs(og))
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
	lda	XX
	sub	#$01
	sta	XX
	bcs	@L014E
	dec	XX+1
@L014E: ; mo = ou; }
	lda	OUkos
	ldx	OUkos+1
	jmp	@L014G
	; else { mo = og }
@L0149: lda	OGora
	ldx	OGora+1
@L014G: sta	MaxO
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
	rts


; ------------------------------------------------------------------------
; OUTTEXT: Output text at X/Y = ptr1/ptr2 using the current color and the
; current text style. The text to output is given as a zero terminated
; string with address in ptr3.
;
; Must set an error code: NO
;

OUTTEXT:
	lda	ptr1
	sta	CH
	lda	ptr2
	jsr	VTABZ
	ldy	#$00
@L1:	lda	(ptr3),y
	ora	#$80
	jsr	COUT
	iny
	cmp	#$80
	bne	@L1
	rts

;-------------
; copies of some runtime routines

abs:
	; a/y := abs(a/y)
	cpy	#$00
	bpl	absend
	; negay
neg:	clc
	eor	#$FF
	adc	#$01
	pha
	tya
	eor	#$FF
	adc	#$00
	tay
	pla
absend:	rts

icmp:
	; compare a/y to zp,x
	sta	TEMP		; TEMP/TEMP2 - arg2
	sty	TEMP2
	lda	$00,x
	pha
	lda	$01,x
	tay
	pla
	tax
	tya			; x/a - arg1 (a=high)

	sub	TEMP2
	bne	@L4
	cpx	TEMP
	beq	@L3
	adc	#$FF
	ora	#$01
@L3:	rts
@L4:	bvc	@L3
	eor	#$FF
	ora	#$01
	rts
