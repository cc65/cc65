;
; Graphics driver for the 280x192x6 mode on the Apple II
;
; Stefan Haubenthal <polluks@sdf.lonestar.org>
; Based on Maciej Witkowiak's circle routine
;

	.include	"zeropage.inc"

	.include	"tgi-kernel.inc"
	.include	"tgi-mode.inc"
	.include	"tgi-error.inc"
	.include	"apple2.inc"

	.macpack	generic

; ------------------------------------------------------------------------
; ROM entry points

TEXT    :=	$F399	; Return to text screen
HGR     :=	$F3E2	; Initialize and clear hi-res page 1.
HCLR    :=	$F3F2	; Clear the current hi-res screen to black.
HPOSN   :=	$F411	; Positions the hi-res cursor without
                        ; plotting a point.
                        ; Enter with (A) = Y-coordinate, and
                        ; (Y,X) = X-coordinate.
HPLOT   :=	$F457	; Calls HPOSN and tries to plot a dot at
                        ; the cursor's position.  If you are
                        ; trying to plot a non-white color at
                        ; a complementary color position, no
                        ; dot will be plotted.
HLIN    :=	$F53A	; Draws a line from the last plotted
                        ; point or line destination to:
                        ; (X,A) = X-coordinate, and
                        ; (Y) = Y-coordinate.
DRAW    :=	$F601	; Draws a shape.  Enter with (Y,X) = the
                        ; address of the shape table, and (A) =
                        ; the rotation factor.  Uses the current
                        ; color.
SETHCOL :=	$F6EC	; Set the hi-res color to (X), where (X)
                        ; must be between 0 and 7.

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
       	.addr   CIRCLE
       	.addr   TEXTSTYLE
       	.addr   OUTTEXT
        .addr   0                       ; IRQ entry is unused

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

SHAPE:	.byte	$64,$01,$D0,$00,$D5,$00,$DA,$00,$E0,$00,$EF,$00,$FE,$00,$0C,$01
	.byte	$19,$01,$1D,$01,$25,$01,$2D,$01,$3D,$01,$46,$01,$4B,$01,$52,$01
	.byte	$56,$01,$60,$01,$70,$01,$77,$01,$83,$01,$8E,$01,$9A,$01,$A7,$01
	.byte	$B6,$01,$BF,$01,$CE,$01,$DD,$01,$E2,$01,$E7,$01,$F9,$01,$03,$02
	.byte	$15,$02,$1F,$02,$32,$02,$42,$02,$52,$02,$5E,$02,$6C,$02,$79,$02
	.byte	$85,$02,$91,$02,$A1,$02,$A9,$02,$B2,$02,$BF,$02,$C5,$02,$D5,$02
	.byte	$E5,$02,$F4,$02,$00,$03,$10,$03,$1F,$03,$2D,$03,$35,$03,$44,$03
	.byte	$53,$03,$63,$03,$71,$03,$7B,$03,$85,$03,$91,$03,$97,$03,$A3,$03
	.byte	$B6,$03,$BF,$03,$C3,$03,$CF,$03,$DC,$03,$E4,$03,$F3,$03,$00,$04
	.byte	$0A,$04,$19,$04,$25,$04,$2A,$04,$32,$04,$3C,$04,$43,$04,$50,$04
	.byte	$5A,$04,$66,$04,$72,$04,$7E,$04,$87,$04,$94,$04,$9C,$04,$A8,$04
	.byte	$B4,$04,$C1,$04,$CC,$04,$DB,$04,$E6,$04,$EE,$04,$F3,$04,$FB,$04
	.byte	$04,$05,$1A,$05,$1F,$05,$24,$05,$29,$05,$22,$01,$00,$00,$00,$00
	.byte	$09,$89,$92,$92,$00,$36,$36,$16,$0E,$00,$0D,$FE,$6E,$96,$52,$00
	.byte	$69,$FE,$17,$2D,$2D,$1E,$1F,$17,$2D,$2D,$1E,$1F,$6E,$4E,$00,$09
	.byte	$8D,$3F,$BF,$0D,$15,$3F,$0E,$0D,$1E,$3F,$77,$71,$09,$00,$6D,$11
	.byte	$DF,$77,$09,$1E,$17,$4D,$3A,$DF,$4E,$29,$0E,$00,$A9,$1F,$6E,$1E
	.byte	$17,$0D,$0D,$1E,$DF,$0E,$6D,$0E,$00,$36,$96,$52,$00,$09,$1E,$17
	.byte	$36,$15,$0E,$0E,$00,$15,$0E,$36,$1E,$17,$4E,$01,$00,$09,$8D,$1F
	.byte	$1F,$0E,$2D,$1E,$17,$2D,$15,$1F,$1F,$4E,$4E,$01,$00,$89,$6E,$3A
	.byte	$3F,$77,$31,$56,$09,$00,$92,$8A,$F6,$0D,$00,$52,$89,$3F,$B7,$52
	.byte	$49,$00,$92,$92,$0E,$00,$49,$11,$17,$1E,$17,$1E,$56,$49,$01,$00
	.byte	$29,$AD,$DF,$33,$4D,$35,$1F,$1F,$2E,$4D,$FE,$1B,$0E,$2D,$4E,$00
	.byte	$31,$77,$36,$BE,$2D,$0E,$00,$29,$AD,$DF,$73,$49,$1E,$BF,$1E,$2E
	.byte	$2D,$75,$00,$2D,$2D,$BE,$3E,$4E,$FE,$1B,$0E,$2D,$4E,$00,$49,$3E
	.byte	$17,$0D,$FE,$33,$2D,$2D,$1E,$76,$01,$00,$2D,$2D,$DE,$1B,$2E,$2D
	.byte	$15,$36,$DF,$73,$2D,$4E,$00,$09,$2D,$DE,$BB,$4D,$3A,$3F,$6E,$09
	.byte	$FE,$1B,$0E,$2D,$4E,$00,$2D,$2D,$BE,$1E,$17,$36,$4E,$09,$00,$29
	.byte	$AD,$DF,$33,$4D,$F1,$3F,$17,$4D,$31,$DF,$73,$2D,$4E,$00,$29,$AD
	.byte	$DF,$33,$4D,$31,$3F,$77,$09,$1E,$DF,$2A,$75,$09,$00,$12,$16,$96
	.byte	$01,$00,$52,$B2,$F6,$0D,$00,$49,$3E,$17,$2D,$2D,$35,$3F,$3F,$3F
	.byte	$0E,$2D,$2D,$F5,$1B,$77,$4E,$09,$00,$12,$2D,$2D,$DE,$9B,$2D,$2D
	.byte	$96,$01,$00,$49,$15,$FF,$13,$2D,$2D,$AD,$3F,$3F,$3F,$2E,$2D,$2D
	.byte	$1E,$37,$4E,$09,$00,$29,$AD,$DF,$73,$09,$1E,$B6,$4E,$01,$00,$09
	.byte	$2D,$3E,$37,$2D,$3E,$FF,$2A,$2D,$2D,$F5,$3F,$3F,$0E,$2D,$1E,$4D
	.byte	$01,$00,$09,$15,$1F,$17,$4D,$31,$DF,$33,$2D,$2D,$FE,$1B,$6E,$09
	.byte	$0E,$00,$2D,$AD,$DF,$33,$4D,$F1,$3F,$37,$4D,$31,$DF,$33,$2D,$75
	.byte	$01,$00,$29,$AD,$DF,$33,$36,$4D,$11,$DF,$73,$2D,$4E,$00,$2D,$AD
	.byte	$DF,$6E,$31,$DF,$6E,$31,$DF,$17,$2D,$75,$01,$00,$2D,$2D,$DE,$1B
	.byte	$6E,$11,$3F,$37,$36,$2D,$2D,$0E,$00,$2D,$2D,$DE,$1B,$6E,$11,$3F
	.byte	$37,$36,$4E,$49,$00,$29,$AD,$DF,$33,$36,$4D,$35,$DF,$73,$2D,$75
	.byte	$00,$4D,$31,$DF,$33,$4D,$31,$3F,$3F,$6E,$09,$FE,$1B,$6E,$09,$0E
	.byte	$00,$2D,$1E,$36,$36,$17,$2D,$0E,$00,$49,$31,$36,$36,$DF,$73,$2D
	.byte	$4E,$00,$4D,$F1,$DF,$6E,$1E,$37,$0D,$15,$DF,$6E,$09,$0E,$00,$36
	.byte	$36,$36,$2D,$75,$00,$4D,$31,$FF,$37,$0D,$0D,$FE,$1F,$6E,$09,$FE
	.byte	$1B,$6E,$09,$0E,$00,$4D,$31,$DF,$33,$6D,$31,$1F,$1F,$6E,$29,$FE
	.byte	$1B,$6E,$09,$0E,$00,$29,$AD,$DF,$33,$4D,$31,$DF,$33,$4D,$31,$DF
	.byte	$73,$2D,$4E,$00,$2D,$AD,$DF,$33,$4D,$F1,$3F,$37,$36,$4E,$49,$00
	.byte	$29,$AD,$DF,$33,$4D,$31,$DF,$33,$0D,$0D,$1E,$DF,$0E,$6D,$0E,$00
	.byte	$2D,$AD,$DF,$33,$4D,$F1,$3F,$37,$0D,$15,$DF,$6E,$09,$0E,$00,$29
	.byte	$AD,$DF,$33,$4D,$3A,$77,$09,$FE,$1B,$0E,$2D,$4E,$00,$2D,$2D,$DE
	.byte	$36,$36,$76,$09,$00,$4D,$31,$DF,$33,$4D,$31,$DF,$33,$4D,$31,$DF
	.byte	$73,$2D,$4E,$00,$4D,$31,$DF,$33,$4D,$31,$DF,$33,$4D,$F1,$1F,$0E
	.byte	$4E,$01,$00,$4D,$31,$DF,$33,$4D,$31,$1F,$1F,$6E,$0D,$3E,$1F,$37
	.byte	$4D,$71,$00,$4D,$31,$DF,$73,$0D,$1E,$17,$0D,$15,$DF,$33,$4D,$71
	.byte	$00,$4D,$31,$DF,$73,$0D,$1E,$36,$76,$09,$00,$2D,$2D,$BE,$1E,$17
	.byte	$1E,$2E,$2D,$75,$00,$2D,$2D,$DE,$3B,$2E,$3E,$2E,$3E,$2E,$2D,$75
	.byte	$00,$72,$15,$0E,$15,$56,$00,$2D,$2D,$3E,$2E,$3E,$2E,$3E,$DF,$2A
	.byte	$2D,$75,$00,$49,$15,$3F,$17,$2D,$2D,$15,$3F,$3F,$3F,$4E,$2D,$3E
	.byte	$37,$2D,$3E,$6F,$49,$00,$92,$92,$49,$11,$3F,$3F,$4D,$09,$00,$76
	.byte	$96,$52,$00,$52,$2D,$35,$DF,$33,$4D,$31,$FF,$73,$6D,$0E,$00,$36
	.byte	$2D,$AD,$DF,$33,$4D,$31,$DF,$33,$2D,$75,$01,$00,$52,$2D,$DE,$33
	.byte	$76,$2D,$0E,$00,$49,$31,$DF,$2A,$2D,$FE,$1B,$6E,$09,$FE,$1B,$0E
	.byte	$2D,$75,$00,$52,$2D,$15,$DF,$33,$2D,$2D,$DE,$1B,$0E,$2D,$75,$00
	.byte	$09,$F5,$33,$8D,$3F,$77,$36,$4E,$01,$00,$52,$2D,$35,$DF,$33,$4D
	.byte	$31,$3F,$77,$09,$1E,$3F,$4D,$01,$00,$36,$2D,$AD,$DF,$33,$4D,$31
	.byte	$DF,$33,$4D,$71,$00,$16,$36,$36,$0E,$00,$09,$9E,$35,$36,$F6,$6F
	.byte	$01,$00,$36,$4D,$1E,$1F,$2E,$15,$1F,$6E,$71,$00,$35,$36,$36,$17
	.byte	$2D,$0E,$00,$12,$2D,$AD,$1F,$1F,$6E,$0D,$FE,$1F,$6E,$0D,$0E,$00
	.byte	$12,$2D,$15,$DF,$6E,$31,$DF,$6E,$71,$00,$52,$2D,$15,$DF,$33,$4D
	.byte	$31,$DF,$73,$2D,$4E,$00,$12,$2D,$AD,$DF,$33,$4D,$F1,$3F,$37,$6E
	.byte	$49,$00,$52,$2D,$35,$DF,$33,$4D,$31,$3F,$77,$09,$2E,$00,$12,$0D
	.byte	$AD,$DF,$37,$36,$4E,$49,$00,$52,$2D,$F5,$DB,$0E,$2D,$15,$DF,$13
	.byte	$2D,$75,$01,$00,$31,$17,$2D,$F5,$33,$76,$75,$00,$12,$4D,$31,$DF
	.byte	$33,$4D,$31,$FF,$73,$6D,$0E,$00,$12,$4D,$31,$DF,$33,$4D,$F1,$1F
	.byte	$0E,$4E,$01,$00,$12,$4D,$31,$DF,$33,$0D,$0D,$FE,$1F,$0E,$0D,$4E
	.byte	$00,$12,$4D,$F1,$1F,$0E,$15,$1F,$17,$4D,$71,$00,$12,$4D,$31,$DF
	.byte	$33,$4D,$31,$3F,$77,$09,$1E,$3F,$4D,$01,$00,$12,$2D,$2D,$1E,$17
	.byte	$1E,$17,$2D,$2D,$0E,$00,$09,$F5,$33,$1E,$0E,$76,$75,$00,$36,$36
	.byte	$36,$2E,$00,$AD,$AE,$17,$FE,$2A,$4E,$01,$00,$69,$11,$1F,$1F,$4E
	.byte	$B1,$92,$09,$00,$2D,$2D,$35,$3F,$3F,$37,$2D,$2D,$35,$3F,$3F,$37
	.byte	$2D,$2D,$35,$3F,$3F,$37,$2D,$2D,$75,$00,$40,$C0,$40,$18,$00,$40
	.byte	$C0,$40,$43,$00,$40,$C0,$40,$08,$00,$19,$00,$00

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
	bit	MIXCLR

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
	bit	HISCR
	rts
@L1:	bit	LOWSCR
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
	lda	#>$4000			; Page 2
	.byte	$2C
@L1:	lda	#>$2000			; Page 1
	sta	$E6
	rts

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
	ldx	X1
	ldy	X1+1
	lda	Y1
	jsr	HPOSN		; 1st pixel
HBASL	=	$26
HMASK	=	$30
	ldx	#$00
	lda	(HBASL),y
	and	HMASK
	beq	@L1
	inx
@L1:	stx	tmp1

	lda	$E0		; which neighbour
	tax
	and	#$01
	bne	@odd
	asl	tmp1
	inx
	.byte	$24
@odd:	dex

	ldy	$E1
	lda	$E2
	jsr	HPOSN		; 2nd pixel
	ldx	#$00
	lda	(HBASL),y
	and	HMASK
	beq	@L2
	inx
@L2:	txa
	ora	tmp1
	ldx	#$00
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
	stx	TEXTMAGX
	sty	TEXTMAGY
	asl				; 16 <=> 90þ
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

	ldy	#$00
@L1:	lda	(ptr3),y
	beq	@end
	sub	#$1F			; no controls
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
	cpy	#$00
	bpl	@L1
	; negay
	clc
	eor	#$FF
	adc	#$01
	pha
	tya
	eor	#$FF
	adc	#$00
	tay
	pla
@L1:	rts

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
