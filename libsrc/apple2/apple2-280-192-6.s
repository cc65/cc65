;
; Graphics driver for the 280x192x6 mode on the Apple II
;
; Stefan Haubenthal <polluks@sdf.lonestar.org>
; Based on Stephen L. Judd's GRLIB code and
; Maciej Witkowiak's circle routine
;

	.include	"zeropage.inc"

	.include	"tgi-kernel.inc"
	.include	"tgi-mode.inc"
	.include	"tgi-error.inc"
	.include	"apple2.inc"

	.macpack	generic

; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

.segment	"JUMPTABLE"

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

	.byte	$74, $67, $69		; "tgi"
	.byte	TGI_API_VERSION		; TGI API version number
xres:	.word	280			; X resolution
yres:	.word	192			; Y resolution
	.byte	8			; Number of drawing colors
	.byte	2			; Number of screens available
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

OGora:		.res	2
OUkos:		.res	2
Y3:		.res	2

; Text output stuff
TEXTMAGX	= $E7
TEXTMAGY	= $E7
TEXTDIR		= $F9

; Constants and tables

.rodata

DEFPALETTE:	.byte	$00, $05, $04, $01, $00, $08, $06, $01; 6 unique colors
PALETTESIZE	= * - DEFPALETTE

SHAPE:  .byte   $64,$01,$d0,$00,$d5,$00,$da,$00,$e0,$00,$ef,$00,$fe,$00,$0c,$01
        .byte   $19,$01,$1d,$01,$25,$01,$2d,$01,$3d,$01,$46,$01,$4b,$01,$52,$01
        .byte   $56,$01,$60,$01,$70,$01,$77,$01,$83,$01,$8e,$01,$9a,$01,$a7,$01
        .byte   $b6,$01,$bf,$01,$ce,$01,$dd,$01,$e2,$01,$e7,$01,$f9,$01,$03,$02
        .byte   $15,$02,$1f,$02,$32,$02,$42,$02,$52,$02,$5e,$02,$6c,$02,$79,$02
        .byte   $85,$02,$91,$02,$a1,$02,$a9,$02,$b2,$02,$bf,$02,$c5,$02,$d5,$02
        .byte   $e5,$02,$f4,$02,$00,$03,$10,$03,$1f,$03,$2d,$03,$35,$03,$44,$03
        .byte   $53,$03,$63,$03,$71,$03,$7b,$03,$85,$03,$91,$03,$97,$03,$a3,$03
        .byte   $b6,$03,$bf,$03,$c3,$03,$cf,$03,$dc,$03,$e4,$03,$f3,$03,$00,$04
        .byte   $0a,$04,$19,$04,$25,$04,$2a,$04,$32,$04,$3c,$04,$43,$04,$50,$04
        .byte   $5a,$04,$66,$04,$72,$04,$7e,$04,$87,$04,$94,$04,$9c,$04,$a8,$04
        .byte   $b4,$04,$c1,$04,$cc,$04,$db,$04,$e6,$04,$ee,$04,$f3,$04,$fb,$04
        .byte   $04,$05,$1a,$05,$1f,$05,$24,$05,$29,$05,$22,$01,$00,$00,$00,$00
        .byte   $09,$89,$92,$92,$00,$36,$36,$16,$0e,$00,$0d,$fe,$6e,$96,$52,$00
        .byte   $69,$fe,$17,$2d,$2d,$1e,$1f,$17,$2d,$2d,$1e,$1f,$6e,$4e,$00,$09
        .byte   $8d,$3f,$bf,$0d,$15,$3f,$0e,$0d,$1e,$3f,$77,$71,$09,$00,$6d,$11
        .byte   $df,$77,$09,$1e,$17,$4d,$3a,$df,$4e,$29,$0e,$00,$a9,$1f,$6e,$1e
        .byte   $17,$0d,$0d,$1e,$df,$0e,$6d,$0e,$00,$36,$96,$52,$00,$09,$1e,$17
        .byte   $36,$15,$0e,$0e,$00,$15,$0e,$36,$1e,$17,$4e,$01,$00,$09,$8d,$1f
        .byte   $1f,$0e,$2d,$1e,$17,$2d,$15,$1f,$1f,$4e,$4e,$01,$00,$89,$6e,$3a
        .byte   $3f,$77,$31,$56,$09,$00,$92,$8a,$f6,$0d,$00,$52,$89,$3f,$b7,$52
        .byte   $49,$00,$92,$92,$0e,$00,$49,$11,$17,$1e,$17,$1e,$56,$49,$01,$00
        .byte   $29,$ad,$df,$33,$4d,$35,$1f,$1f,$2e,$4d,$fe,$1b,$0e,$2d,$4e,$00
        .byte   $31,$77,$36,$be,$2d,$0e,$00,$29,$ad,$df,$73,$49,$1e,$bf,$1e,$2e
        .byte   $2d,$75,$00,$2d,$2d,$be,$3e,$4e,$fe,$1b,$0e,$2d,$4e,$00,$49,$3e
        .byte   $17,$0d,$fe,$33,$2d,$2d,$1e,$76,$01,$00,$2d,$2d,$de,$1b,$2e,$2d
        .byte   $15,$36,$df,$73,$2d,$4e,$00,$09,$2d,$de,$bb,$4d,$3a,$3f,$6e,$09
        .byte   $fe,$1b,$0e,$2d,$4e,$00,$2d,$2d,$be,$1e,$17,$36,$4e,$09,$00,$29
        .byte   $ad,$df,$33,$4d,$f1,$3f,$17,$4d,$31,$df,$73,$2d,$4e,$00,$29,$ad
        .byte   $df,$33,$4d,$31,$3f,$77,$09,$1e,$df,$2a,$75,$09,$00,$12,$16,$96
        .byte   $01,$00,$52,$b2,$f6,$0d,$00,$49,$3e,$17,$2d,$2d,$35,$3f,$3f,$3f
        .byte   $0e,$2d,$2d,$f5,$1b,$77,$4e,$09,$00,$12,$2d,$2d,$de,$9b,$2d,$2d
        .byte   $96,$01,$00,$49,$15,$ff,$13,$2d,$2d,$ad,$3f,$3f,$3f,$2e,$2d,$2d
        .byte   $1e,$37,$4e,$09,$00,$29,$ad,$df,$73,$09,$1e,$b6,$4e,$01,$00,$09
        .byte   $2d,$3e,$37,$2d,$3e,$ff,$2a,$2d,$2d,$f5,$3f,$3f,$0e,$2d,$1e,$4d
        .byte   $01,$00,$09,$15,$1f,$17,$4d,$31,$df,$33,$2d,$2d,$fe,$1b,$6e,$09
        .byte   $0e,$00,$2d,$ad,$df,$33,$4d,$f1,$3f,$37,$4d,$31,$df,$33,$2d,$75
        .byte   $01,$00,$29,$ad,$df,$33,$36,$4d,$11,$df,$73,$2d,$4e,$00,$2d,$ad
        .byte   $df,$6e,$31,$df,$6e,$31,$df,$17,$2d,$75,$01,$00,$2d,$2d,$de,$1b
        .byte   $6e,$11,$3f,$37,$36,$2d,$2d,$0e,$00,$2d,$2d,$de,$1b,$6e,$11,$3f
        .byte   $37,$36,$4e,$49,$00,$29,$ad,$df,$33,$36,$4d,$35,$df,$73,$2d,$75
        .byte   $00,$4d,$31,$df,$33,$4d,$31,$3f,$3f,$6e,$09,$fe,$1b,$6e,$09,$0e
        .byte   $00,$2d,$1e,$36,$36,$17,$2d,$0e,$00,$49,$31,$36,$36,$df,$73,$2d
        .byte   $4e,$00,$4d,$f1,$df,$6e,$1e,$37,$0d,$15,$df,$6e,$09,$0e,$00,$36
        .byte   $36,$36,$2d,$75,$00,$4d,$31,$ff,$37,$0d,$0d,$fe,$1f,$6e,$09,$fe
        .byte   $1b,$6e,$09,$0e,$00,$4d,$31,$df,$33,$6d,$31,$1f,$1f,$6e,$29,$fe
        .byte   $1b,$6e,$09,$0e,$00,$29,$ad,$df,$33,$4d,$31,$df,$33,$4d,$31,$df
        .byte   $73,$2d,$4e,$00,$2d,$ad,$df,$33,$4d,$f1,$3f,$37,$36,$4e,$49,$00
        .byte   $29,$ad,$df,$33,$4d,$31,$df,$33,$0d,$0d,$1e,$df,$0e,$6d,$0e,$00
        .byte   $2d,$ad,$df,$33,$4d,$f1,$3f,$37,$0d,$15,$df,$6e,$09,$0e,$00,$29
        .byte   $ad,$df,$33,$4d,$3a,$77,$09,$fe,$1b,$0e,$2d,$4e,$00,$2d,$2d,$de
        .byte   $36,$36,$76,$09,$00,$4d,$31,$df,$33,$4d,$31,$df,$33,$4d,$31,$df
        .byte   $73,$2d,$4e,$00,$4d,$31,$df,$33,$4d,$31,$df,$33,$4d,$f1,$1f,$0e
        .byte   $4e,$01,$00,$4d,$31,$df,$33,$4d,$31,$1f,$1f,$6e,$0d,$3e,$1f,$37
        .byte   $4d,$71,$00,$4d,$31,$df,$73,$0d,$1e,$17,$0d,$15,$df,$33,$4d,$71
        .byte   $00,$4d,$31,$df,$73,$0d,$1e,$36,$76,$09,$00,$2d,$2d,$be,$1e,$17
        .byte   $1e,$2e,$2d,$75,$00,$2d,$2d,$de,$3b,$2e,$3e,$2e,$3e,$2e,$2d,$75
        .byte   $00,$72,$15,$0e,$15,$56,$00,$2d,$2d,$3e,$2e,$3e,$2e,$3e,$df,$2a
        .byte   $2d,$75,$00,$49,$15,$3f,$17,$2d,$2d,$15,$3f,$3f,$3f,$4e,$2d,$3e
        .byte   $37,$2d,$3e,$6f,$49,$00,$92,$92,$49,$11,$3f,$3f,$4d,$09,$00,$76
        .byte   $96,$52,$00,$52,$2d,$35,$df,$33,$4d,$31,$ff,$73,$6d,$0e,$00,$36
        .byte   $2d,$ad,$df,$33,$4d,$31,$df,$33,$2d,$75,$01,$00,$52,$2d,$de,$33
        .byte   $76,$2d,$0e,$00,$49,$31,$df,$2a,$2d,$fe,$1b,$6e,$09,$fe,$1b,$0e
        .byte   $2d,$75,$00,$52,$2d,$15,$df,$33,$2d,$2d,$de,$1b,$0e,$2d,$75,$00
        .byte   $09,$f5,$33,$8d,$3f,$77,$36,$4e,$01,$00,$52,$2d,$35,$df,$33,$4d
        .byte   $31,$3f,$77,$09,$1e,$3f,$4d,$01,$00,$36,$2d,$ad,$df,$33,$4d,$31
        .byte   $df,$33,$4d,$71,$00,$16,$36,$36,$0e,$00,$09,$9e,$35,$36,$f6,$6f
        .byte   $01,$00,$36,$4d,$1e,$1f,$2e,$15,$1f,$6e,$71,$00,$35,$36,$36,$17
        .byte   $2d,$0e,$00,$12,$2d,$ad,$1f,$1f,$6e,$0d,$fe,$1f,$6e,$0d,$0e,$00
        .byte   $12,$2d,$15,$df,$6e,$31,$df,$6e,$71,$00,$52,$2d,$15,$df,$33,$4d
        .byte   $31,$df,$73,$2d,$4e,$00,$12,$2d,$ad,$df,$33,$4d,$f1,$3f,$37,$6e
        .byte   $49,$00,$52,$2d,$35,$df,$33,$4d,$31,$3f,$77,$09,$2e,$00,$12,$0d
        .byte   $ad,$df,$37,$36,$4e,$49,$00,$52,$2d,$f5,$db,$0e,$2d,$15,$df,$13
        .byte   $2d,$75,$01,$00,$31,$17,$2d,$f5,$33,$76,$75,$00,$12,$4d,$31,$df
        .byte   $33,$4d,$31,$ff,$73,$6d,$0e,$00,$12,$4d,$31,$df,$33,$4d,$f1,$1f
        .byte   $0e,$4e,$01,$00,$12,$4d,$31,$df,$33,$0d,$0d,$fe,$1f,$0e,$0d,$4e
        .byte   $00,$12,$4d,$f1,$1f,$0e,$15,$1f,$17,$4d,$71,$00,$12,$4d,$31,$df
        .byte   $33,$4d,$31,$3f,$77,$09,$1e,$3f,$4d,$01,$00,$12,$2d,$2d,$1e,$17
        .byte   $1e,$17,$2d,$2d,$0e,$00,$09,$f5,$33,$1e,$0e,$76,$75,$00,$36,$36
        .byte   $36,$2e,$00,$ad,$ae,$17,$fe,$2a,$4e,$01,$00,$69,$11,$1f,$1f,$4e
        .byte   $b1,$92,$09,$00,$2d,$2d,$35,$3f,$3f,$37,$2d,$2d,$35,$3f,$3f,$37
        .byte   $2d,$2d,$35,$3f,$3f,$37,$2d,$2d,$75,$00,$40,$c0,$40,$18,$00,$40
        .byte   $c0,$40,$43,$00,$40,$c0,$40,$08,$00,$19,$00,$00

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

	jsr	HGR
	lda	MIXOFF

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

CLEAR		= HCLR

; ------------------------------------------------------------------------
; SETVIEWPAGE: Set the visible page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;

SETVIEWPAGE:
	tax
	beq	@L1
	lda	PG2ON
	rts
@L1:	lda	PG2OFF
	rts

; ------------------------------------------------------------------------
; SETDRAWPAGE: Set the drawable page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;

SETDRAWPAGE:
	tax
	beq	@L1
	jmp	HGR2
@L1:	jmp	HGR

; ------------------------------------------------------------------------
; SETCOLOR: Set the drawing color (in A). The new color is already checked
; to be in a valid range (0..maxcolor-1).
;
; Must set an error code: NO (will only be called if color ok)
;

SETCOLOR:
	tax
	jmp	SETHCOL

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
	ldx	X1
	ldy	X1+1
	lda	Y1
	jmp	HPLOT

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.


GETPIXEL:
	lda	#TGI_ERR_INV_FUNC
	sta	ERROR
	rts

; ------------------------------------------------------------------------
; LINE: Draw a line from X1/Y1 to X2/Y2, where X1/Y1 = ptr1/ptr2 and
; X2/Y2 = ptr3/ptr4 using the current drawing color.
;
; Must set an error code: NO
;

LINE:
	ldx	X1
	ldy	X1+1
	lda	Y1
	jsr	HPOSN
	lda	X2
	ldx	X2+1
	ldy	Y2
	jmp	HLIN

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
@L1:	lda	Y2
	pha
  	lda	Y1
	sta	Y2
	jsr	LINE
	pla
	sta	Y2
	inc	Y1
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
	add	#1
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
	add	#1
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
	sub	#1
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
	stx	TEXTMAGX
	sty	TEXTMAGY
	asl				; 16 <=> 90°
	asl
	asl
	asl
	sta	TEXTDIR
	rts


; ------------------------------------------------------------------------
; OUTTEXT: Output text at X/Y = ptr1/ptr2 using the current color and the
; current text style. The text to output is given as a zero terminated
; string with address in ptr3.
;
; Must set an error code: NO
;

OUTTEXT:
	ldx	X1
	ldy	X1+1
	lda	Y1
	jsr	HPOSN
	lda	SHAPE+2*99
	add	#<SHAPE
	sta	tmp3
	lda	SHAPE+2*99+1
	adc	#>SHAPE
	sta	tmp3+1

	ldy	#0
@L1:	lda	(ptr3),y
	beq	@end
	sub	#31			; no controls
	asl				; offset*2
	tax
	lda	SHAPE,x
	add	#<SHAPE
	sta	tmp1
	lda	SHAPE+1,x
	adc	#>SHAPE
	sta	tmp1+1
	tya
	pha
	ldx	tmp1
	ldy	tmp1+1
	lda	TEXTDIR
	jsr	DRAW
	ldx	tmp3
	ldy	tmp3+1
	lda	TEXTDIR
	jsr	DRAW
	pla
	tay
	iny
	bne	@L1
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
