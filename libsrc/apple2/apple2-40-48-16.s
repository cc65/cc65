;
; Graphics driver for the 40x48x16 mode on the Apple II
;
; Stefan Haubenthal <polluks@sdf.lonestar.org>
; Oliver Schmidt <ol.sc@web.de>
;

	.include	"zeropage.inc"

	.include	"tgi-kernel.inc"
	.include	"tgi-mode.inc"
	.include	"tgi-error.inc"
	.include	"apple2.inc"

	.macpack	generic

; ------------------------------------------------------------------------

; Zero page stuff

H2	:=	$2C

; ROM entry points

TEXT    :=	$F399
PLOT	:=	$F800
HLINE	:=	$F819
CLRSCR	:=	$F832
SETCOL	:=	$F864
SCRN	:=	$F871
SETGR	:=	$FB40
HOME	:=	$FC58

; ------------------------------------------------------------------------

; Variables mapped to the zero page segment variables. Some of these are
; used for passing parameters to the driver.

X1	:=	ptr1
Y1	:=	ptr2
X2	:=	ptr3
Y2	:=	ptr4

TEMP	:=	tmp3
TEMP2	:=	tmp4
TEMP3	:=	sreg
TEMP4	:=	sreg+1

; Line routine stuff (must be on zpage)

PB 	:=	ptr3 		; (2)	LINE
UB	:=	ptr4 		; (2)	LINE
ERR	:=	regsave 	; (2)	LINE
NX	:=	regsave+2 	; (2)	LINE

; ------------------------------------------------------------------------

	.segment	"JUMPTABLE"

; Header. Includes jump table and constants.

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

	.byte	$74, $67, $69	; "tgi"
	.byte	TGI_API_VERSION	; TGI API version number
xres:	.word	40		; X resolution
yres:	.word	48		; Y resolution
	.byte	16		; Number of drawing colors
	.byte	1		; Number of screens available
	.byte	8		; System font X size
	.byte	8		; System font Y size
	.word   $100		; Aspect ratio

; Next comes the jump table. With the exception of IRQ, all entries must be
; valid and may point to an RTS for test versions (function not implemented).

       	.addr   INSTALL
       	.addr   UNINSTALL
       	.addr   INIT
       	.addr   DONE
       	.addr   GETERROR
       	.addr   CONTROL
       	.addr   CLEAR
       	.addr   SETVIEWPAGE
       	.addr   SETDRAWPAGE
       	.addr   SETCOLOR
       	.addr   SETPALETTE
       	.addr   GETPALETTE
       	.addr   GETDEFPALETTE
       	.addr   SETPIXEL
       	.addr   GETPIXEL
       	.addr   LINE
       	.addr   BAR
       	.addr   TEXTSTYLE
       	.addr   OUTTEXT
        .addr   0               ; IRQ entry is unused

; ------------------------------------------------------------------------

	.bss

; Absolute variables used in the code

ERROR:	.res	1		; Error code

; Line routine stuff (combined with circle routine stuff to save space)

OGora:
COUNT:	.res	2
OUkos:
NY:	.res	2
Y3:
DX:	.res	1
DY:	.res	1
AX:	.res	1
AY:	.res	1

; ------------------------------------------------------------------------

	.rodata

; Constants and tables

DEFPALETTE: .byte $00, $01, $02, $03, $04, $05, $06, $07
	    .byte $08, $09, $0A, $0B, $0C, $0D, $0E, $0F

; ------------------------------------------------------------------------

	.code

; INIT: Changes an already installed device from text mode to graphics mode.
; Note that INIT/DONE may be called multiple times while the driver
; is loaded, while INSTALL is only called once, so any code that is needed
; to initializes variables and so on must go here. Setting palette and
; clearing the screen is not needed because this is called by the graphics
; kernel later.
; The graphics kernel will never call INIT when a graphics mode is already
; active, so there is no need to protect against that.
; Must set an error code: YES
INIT:
	; Switch into graphics mode
	bit	$C082		; Switch in ROM
	jsr	SETGR
	bit	MIXCLR
	bit	$C080		; Switch in LC bank 2 for R/O

	; Done, reset the error code
	lda	#TGI_ERR_OK
	sta	ERROR

	; Fall through

; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
; Must set an error code: NO
INSTALL:
	; Fall through

; UNINSTALL routine. Is called before the driver is removed from memory. May
; clean up anything done by INSTALL but is probably empty most of the time.
; Must set an error code: NO
UNINSTALL:
	; Fall through

; SETVIEWPAGE: Set the visible page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
; Must set an error code: NO (will only be called if page ok)
SETVIEWPAGE:
	; Fall through

; SETDRAWPAGE: Set the drawable page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
; Must set an error code: NO (will only be called if page ok)
SETDRAWPAGE:
	; Fall through

; TEXTSTYLE: Set the style used when calling OUTTEXT. Text scaling in X and Y
; direction is passend in X/Y, the text direction is passed in A.
; Must set an error code: NO
TEXTSTYLE:
	; Fall through

; OUTTEXT: Output text at X/Y = ptr1/ptr2 using the current color and the
; current text style. The text to output is given as a zero terminated
; string with address in ptr3.
; Must set an error code: NO
OUTTEXT:
	rts

; DONE: Will be called to switch the graphics device back into text mode.
; The graphics kernel will never call DONE when no graphics mode is active,
; so there is no need to protect against that.
; Must set an error code: NO
DONE:
	bit	$C082		; Switch in ROM
	jsr	TEXT
	jsr	HOME
	bit	$C080		; Switch in LC bank 2 for R/O
	rts

; GETERROR: Return the error code in A and clear it.
GETERROR:
	lda	ERROR
	ldx	#TGI_ERR_OK
	stx	ERROR
	rts

; CLEAR: Clears the screen.
; Must set an error code: NO
CLEAR:
	bit	$C082		; Switch in ROM
	jsr	CLRSCR
	bit	$C080		; Switch in LC bank 2 for R/O
	rts

; SETCOLOR: Set the drawing color (in A). The new color is already checked
; to be in a valid range (0..maxcolor-1).
; Must set an error code: NO (will only be called if color ok)
SETCOLOR:
	bit	$C082		; Switch in ROM
	jsr	SETCOL
	bit	$C080		; Switch in LC bank 2 for R/O
	rts

; CONTROL: Platform/driver specific entry point.
; Must set an error code: YES
CONTROL:
	; Fall through

; SETPALETTE: Set the palette (not available with all drivers/hardware).
; A pointer to the palette is passed in ptr1. Must set an error if palettes
; are not supported
; Must set an error code: YES
SETPALETTE:
	lda	#TGI_ERR_INV_FUNC
	sta	ERROR
	rts

; GETPALETTE: Return the current palette in A/X. Even drivers that cannot
; set the palette should return the default palette here, so there's no
; way for this function to fail.
; Must set an error code: NO
GETPALETTE:
	; Fall through

; GETDEFPALETTE: Return the default palette for the driver in A/X. All
; drivers should return something reasonable here, even drivers that don't
; support palettes, otherwise the caller has no way to determine the colors
; of the (not changeable) palette.
; Must set an error code: NO (all drivers must have a default palette)
GETDEFPALETTE:
	lda	#<DEFPALETTE
	ldx	#>DEFPALETTE
	rts

; SETPIXEL: Draw one pixel at X1/Y1 = ptr1/ptr2 with the current drawing
; color. The coordinates passed to this function are never outside the
; visible screen area, so there is no need for clipping inside this function.
; Must set an error code: NO
SETPIXEL:
	bit	$C082		; Switch in ROM
	ldy	X1
	lda	Y1
	jsr	PLOT
	bit	$C080		; Switch in LC bank 2 for R/O
	rts

; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.
GETPIXEL:
	bit	$C082		; Switch in ROM
	ldy	X1
	lda	Y1
	jsr	SCRN
	ldx	#$00
	bit	$C080		; Switch in LC bank 2 for R/O
	rts

; LINE: Draw a line from X1/Y1 to X2/Y2, where X1/Y1 = ptr1/ptr2 and
; X2/Y2 = ptr3/ptr4 using the current drawing color.
; Must set an error code: NO
LINE:
	; nx = abs (x2 - x1)
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

	; ny = abs (y2 - y1)
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

	; if (x2 >= x1)
	ldx	#X2
	lda	X1
	ldy	X1+1
	jsr	icmp
	bcc	:+

	;    dx = 1
	lda	#$01
	bne	:++

	; else
	;    dx = -1
:	lda	#$FF
:	sta	DX

	; if (y2 >= y1)
	ldx	#Y2
	lda	Y1
	ldy	Y1+1
	jsr	icmp
	bcc	:+

	;    dy = 1
	lda	#$01
	bne	:++

	; else
	;    dy = -1
:	lda	#$FF
:	sta	DY

	; err = ax = ay = 0
	lda	#$00
	sta	ERR
	sta	ERR+1
	sta	AX
	sta	AY

	; if (nx < ny) {
	ldx	#NX
	lda	NY
	ldy	NY+1
	jsr	icmp
	bcs	:+

	;    nx <-> ny
	lda	NX
	ldx	NY
	sta	NY
	stx	NX
	lda	NX+1
	ldx	NY+1
	sta	NY+1
	stx	NX+1

	;    ax = dx
	lda	DX
	sta	AX

	;    ay = dy
	lda	DY
	sta	AY

	;    dx = dy = 0 }
	lda	#$00
	sta	DX
	sta	DY

	; ny = - ny
:	lda	NY
	ldy	NY+1
	jsr	neg
	sta	NY
	sty	NY+1

	; for (count = nx; count > 0; --count) {
	lda	NX
	ldx	NX+1
	sta	COUNT
	stx	COUNT+1
for:	lda	COUNT		; count > 0
	ora	COUNT+1
	bne	:+
	rts

	;    setpixel (X1, Y1)
:	jsr	SETPIXEL

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
	bpl	:+
	dex
:	add	X1
	sta	X1
	txa
	adc	X1+1
	sta	X1+1

	;    y1 = y1 + ay
	ldx	#$00
	lda	AY
	bpl	:+
	dex
:	add	Y1
	sta	Y1
	txa
	adc	Y1+1
	sta	Y1+1

	;    if (abs (pb) < abs (ub)) {
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
	bpl	:+

	;       err = pb }
	lda	PB
	ldx	PB+1
	jmp	next

	;    else { x1 = x1 + ax
:	ldx	#$00
	lda	AX
	bpl	:+
	dex
:	add	X1
	sta	X1
	txa
	adc	X1+1
	sta	X1+1

	;       y1 = y1 + dy
	ldx	#$00
	lda	DY
	bpl	:+
	dex
:	add	Y1
	sta	Y1
	txa
	adc	Y1+1
	sta	Y1+1

	;	err = ub }
	lda	UB
	ldx	UB+1
next:	sta	ERR
	stx	ERR+1

	; } (--count)
	lda	COUNT
	sub	#$01
	sta	COUNT
	bcc	:+
	jmp	for
:	dec	COUNT+1
	jmp	for

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
; Must set an error code: NO
BAR:
	bit	$C082		; Switch in ROM
	inc	Y2
	ldx	X2
	stx	H2
:	ldy	X1
	lda	Y1
	jsr	HLINE
	inc	Y1
	lda	Y2
	cmp	Y1
	bne	:-
	bit	$C080		; Switch in LC bank 2 for R/O
	rts

; Copies of some runtime routines

abs:
	; A/Y := abs (A/Y)
	cpy	#$00
	bpl	:+
	
	; A/Y := neg (A/Y)
neg:	clc
	eor	#$FF
	adc	#$01
	pha
	tya
	eor	#$FF
	adc	#$00
	tay
	pla
:	rts

icmp:
	; Compare A/Y to zp,X
	sta	TEMP		; TEMP/TEMP2 - arg2
	sty	TEMP2
	lda	$00,x
	pha
	lda	$01,x
	tay
	pla
	tax
	tya			; X/A - arg1 (a = high)

	sub	TEMP2
	bne	:++
	cpx	TEMP
	beq	:+
	adc	#$FF
	ora	#$01
:	rts
:	bvc	:+
	eor	#$FF
	ora	#$01
:	rts
