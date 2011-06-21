;
; Graphics driver for the lores 64x56x2 mode on the NES
;
; Stefan Haubenthal, 2009-03-10
; Based on Maciej Witkowiak's line routine.
;

	.include	"zeropage.inc"

	.include	"tgi-kernel.inc"
	.include	"tgi-error.inc"
	.include	"nes.inc"
	.include	"get_tv.inc"
	.import		_clrscr, setcursor, putchar
	.import		paldata

	.macpack	generic

; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

.segment	"JUMPTABLE"

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

	.byte	$74, $67, $69		; "tgi"
	.byte	TGI_API_VERSION		; TGI API version number
xres:	.word	charsperline*2		; Max X resolution
yres:	.word	56			; Max Y resolution
	.byte	2			; Number of drawing colors
	.byte	1			; Number of screens available
	.byte	2			; System font X size
	.byte	2			; System font Y size
	.word	$100			; Aspect ratio

; Next comes the jump table. Currently all entries must be valid and may point
; to an RTS for test versions (function not implemented).

	.addr	INSTALL
	.addr	UNINSTALL
	.addr	INIT
	.addr	DONE
	.addr	GETERROR
	.addr	CONTROL
	.addr	CLEAR
	.addr	SETVIEWPAGE
	.addr	SETDRAWPAGE
	.addr	SETCOLOR
	.addr	SETPALETTE
	.addr	GETPALETTE
	.addr	GETDEFPALETTE
	.addr	SETPIXEL
	.addr	GETPIXEL
	.addr	LINE
	.addr	BAR
	.addr	TEXTSTYLE
	.addr	OUTTEXT
	.addr	0			; IRQ entry is unused

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
PB		= ptr3		; (2)	LINE
UB		= ptr4		; (2)	LINE
ERR		= regsave	; (2)	LINE
NX		= regsave+2	; (2)	LINE

; Absolute variables used in the code

.bss

MEM:		.res	charsperline*2*56/4
MEMEND:
ERROR:		.res	1	; Error code
COLOR:		.res	1	; Current color
PALETTE:	.res	2	; The current palette

; Line routine stuff

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

DEFPALETTE:	.byte	$0, $1
OFFSET:		.byte	8, 4, 2, 1
;			00  00  00  00  01  01  01  01
;			00  01  10  11  00  01  10  11
CODE:		.byte	32, 29, 26, 25, 28, 24+128, 31+128, 30+128
;			10  10  10  10  11  11  11  11
;			00  01  10  11  00  01  10  11
		.byte	30, 31, 24, 28+128, 25+128, 26+128, 29+128, 32+128

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
;
; Must set an error code: NO
;

INSTALL:
	jsr	_get_tv
	cmp	#TV::NTSC
	beq	ntsc
; TODO ROM!
	inc	yres
	inc	yres
ntsc:;	rts

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

; Done, reset the error code

	lda	#TGI_ERR_OK
	sta	ERROR
;	rts

; ------------------------------------------------------------------------
; DONE: Will be called to switch the graphics device back into text mode.
; The graphics kernel will never call DONE when no graphics mode is active,
; so there is no need to protect against that.
;
; Must set an error code: NO
;

DONE:
	rts

; ------------------------------------------------------------------------
; GETERROR: Return the error code in A and clear it.

GETERROR:
	lda	ERROR
	ldx	#TGI_ERR_OK
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
	ldx	#<MEM
	stx	TEMP
	ldx	#>MEM
	stx	TEMP+1
	lda	#0
	tay
@L1:	sta	(TEMP),y
	iny
	bne	@L1
	inc	TEMP+1
	inx
	cpx	#>MEMEND
	bne	@L1
	jmp	_clrscr

; ------------------------------------------------------------------------
; SETCOLOR: Set the drawing color (in A). The new color is already checked
; to be in a valid range (0..maxcolor-1).
;
; Must set an error code: NO (will only be called if color ok)
;

SETCOLOR:
	sta	COLOR
;	rts

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
; SETPALETTE: Set the palette (not available with all drivers/hardware).
; A pointer to the palette is passed in ptr1. Must set an error if palettes
; are not supported
;
; Must set an error code: YES
;

SETPALETTE:
; Wait for v-blank
@wait:	lda	PPU_STATUS
	bpl	@wait

	lda	#$3F
	sta	PPU_VRAM_ADDR2
	lda	#$00
	sta	PPU_VRAM_ADDR2

	ldy	#0
	lda	(ptr1),y
	sta	PALETTE
	tax
	lda	paldata,x
;	sta	PPU_VRAM_IO

	iny
	lda	(ptr1),y
	sta	PALETTE+1
	tax
	lda	paldata,x
	sta	PPU_VRAM_IO

	lda	#TGI_ERR_OK
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
	lda	#<PALETTE
	ldx	#>PALETTE
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

SETPIXEL:
	ldx	Y1		; y+2<yres
	inx
	inx
	cpx	yres
	bcc	@L2
	ldx	X1		; x+2<xres
	inx
	inx
	cpx	xres
	bcc	@L2
	rts
@L2:	lda	X1
	lsr
	tay
	lda	Y1
	lsr
	tax
	clc
	jsr	setcursor
	jsr	CALC
	ldx	COLOR
	bne	@set2
	eor	#%00001111
@set2:	sta	TEMP3

	lda	(TEMP),y
	ldx	COLOR
	bne	@set
	and	TEMP3
	.byte	$2c
@set:	ora	TEMP3
	sta	(TEMP),y
	tax
	lda	CODE,x
@normal:jmp	putchar

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.


GETPIXEL:
	jsr	CALC
	sta	TEMP3
	lda	(TEMP),y
	and	TEMP3
	beq	@L1
	lda	#1
@L1:	ldx	#0
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
	; if (x2>=x1)
	ldx	#X2
	lda	X1
	ldy	#0
	jsr	icmp
	bcc	@L0243
	beq	@L0243
	; dx = 1;
	lda	#1
	bne	@L0244
	; else
	; dx = -1;
@L0243: lda	#$ff
@L0244: sta	DX
	; if (y2>=y1)
	ldx	#Y2
	lda	Y1
	ldy	#0
	jsr	icmp
	bcc	@L024A
	beq	@L024A
	; dy = 1;
	lda	#1
	bne	@L024B
	; else
	; dy = -1;
@L024A: lda	#$ff
@L024B: sta	DY
	; err = ay = 0;
	lda	#0
	sta	ERR
	sta	ERR+1
	sta	AY

	; if (nx<ny) {
	ldx	#NX
	lda	NY
	ldy	#0
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
	lda	#0
	sta	DX
	sta	DY
	; ny = - ny;
@L0255: lda	NY
	ldy	#0
	jsr	neg
	sta	NY
	sty	NY+1
	; for (count=nx;count>0;--count) {
	lda	NX
	ldx	#0
	sta	COUNT
	stx	COUNT+1
@L0166: lda	COUNT		; count>0
	ora	COUNT+1
	bne	@L0167
	rts
	;    setpixel(X1,Y1)
@L0167: jsr	SETPIXEL
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
	ldx	#0
	lda	DX
	bpl	@L027B
	dex
@L027B: add	X1
	sta	X1
	txa
	adc	X1+1
	sta	X1+1
	;   y1 = y1 + ay
	ldx	#0
	lda	AY
	bpl	@L027E
	dex
@L027E: add	Y1
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
	ldx	#0
	lda	AY
	bpl	@L0288
	dex
@L0288: add	X1
	sta	X1
	txa
	adc	X1+1
	sta	X1+1
	;	y1 = y1 + dy
	ldx	#0
	lda	DY
	bpl	@L028B
	dex
@L028B: add	Y1
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
	sbc	#1
	sta	COUNT
	bcc	@L0260
	jmp	@L0166
@L0260: dec	COUNT+1
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
@L1:	lda	X1
	pha
@L2:	jsr	SETPIXEL
	inc	X1
	lda	X2
	cmp	X1
	bne	@L2
	pla
	sta	X1
	inc	Y1
	lda	Y2
	cmp	Y1
	bne	@L1
	rts

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
	lsr
	tay
	lda	ptr2
	lsr
	tax
	clc
	jsr	setcursor
	ldy	#0
@L1:	lda	(ptr3),y
	jsr	putchar
	iny
	cmp	#$0
	bne	@L1
	rts

;-------------
; copies of some runtime routines

abs:
	; a/y := abs(a/y)
	cpy	#0
	bpl	absend
	; negay
neg:	eor	#$ff
	add	#1
	pha
	tya
	eor	#$ff
	adc	#0
	tay
	pla
absend: rts

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

	sub	TEMP2
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


; ------------------------------------------------------------------------
; Calculate all variables to plot the pixel at X1/Y1. If the point is out
; of range, a carry is returned and INRANGE is set to a value !0 zero. If
; the coordinates are valid, INRANGE is zero and the carry clear.

CALC:	lda	xres
	sta	TEMP
	lda	#0
	sta	TEMP+1
	ldy	Y1
@L1:	lda	TEMP
	add	xres
	lsr
	sta	TEMP
	lda	TEMP+1
	adc	#0
	lsr
	sta	TEMP+1
	dey
	bne	@L1
	lda	TEMP
	add	X1
	lsr
	sta	TEMP
	lda	TEMP+1
	adc	#0
	lsr
	sta	TEMP+1
	lda	TEMP
	add	#<MEM
	sta	TEMP
	lda	TEMP+1
	adc	#>MEM
	sta	TEMP+1

	lda	X1
	and	#%00000001
	sta	TEMP3
	lda	Y1
	asl
	and	#%00000010
	ora	TEMP3
	tax
	lda	OFFSET,x
	rts
