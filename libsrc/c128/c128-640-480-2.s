;
; Graphics driver for the 640x480x2 mode on the C128 VDC 64k
; (values for this mode based on Fred Bowen's document)
; Maciej 'YTM/Elysium' Witkowiak <ytm@elysium.pl>
; 23.12.2002
; 2004-04-04, Greg King
;
; NOTES:
; For any smart monkey that will try to optimize this: PLEASE do tests on
; real VDC, not only VICE.
;
; Only DONE routine contains C128-mode specific stuff, everything else will
; work in C64-mode of C128 (C64 needs full VDC init then).
;
; With special initialization and CALC we can get 320x200 double-pixel mode.
;
; Color translation values for BROWN and GRAY3 are obviously wrong, they
; could be replaced by equiv. of ORANGE and GRAY2 but this would give only
; 14 of 16 colors available.
;
; Register 25 ($19) is said to require different value for VDC v1, but I
; couldn't find what it should be.

	.include     	"zeropage.inc"

      	.include     	"tgi-kernel.inc"
        .include        "tgi-mode.inc"
        .include        "tgi-error.inc"


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
        .byte   TGI_API_VERSION         ; TGI version number
xres:   .word   640                     ; X resolution
yres:   .word   480                     ; Y resolution
        .byte   2                       ; Number of drawing colors
pages:	.byte   0                       ; Number of screens available
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

ADDR		= tmp1		; (2)	CALC
TEMP		= tmp3		;	CALC icmp
TEMP2		= tmp4		;	icmp
TEMP3		= sreg		;	LINE
TEMP4		= sreg+1	;	LINE

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

ERROR:  	.res	1     	; Error code
PALETTE:        .res    2       ; The current palette

BITMASK:        .res    1       ; $00 = clear, $FF = set pixels

OLDCOLOR:	.res	1	; colors before entering gfx mode

; Line routine stuff (combined with CIRCLE to save space)

OGora:
COUNT:		 .res	2
OUkos:
NY:		 .res	2
Y3:
DX:		 .res	1
DY:		 .res	1
AY:		 .res	1

; Text output stuff
TEXTMAGX:       .res    1
TEXTMAGY:       .res    1
TEXTDIR:        .res    1

; Constants and tables

.rodata

DEFPALETTE:     .byte   $00, $0f        ; White on black
PALETTESIZE     = * - DEFPALETTE

BITTAB:         .byte   $80,$40,$20,$10,$08,$04,$02,$01

BITMASKL:	.byte	%11111111, %01111111, %00111111, %00011111
		.byte   %00001111, %00000111, %00000011, %00000001

BITMASKR:	.byte	%10000000, %11000000, %11100000, %11110000
		.byte	%11111000, %11111100, %11111110, %11111111

; color translation table (indexed by VIC color)
COLTRANS:	.byte $00, $0f, $08, $06, $0a, $04, $02, $0c
		.byte $0d, $0b, $09, $01, $0e, $05, $03, $07
		; colors BROWN and GRAY3 are wrong

; VDC initialization table (reg),(val),...,$ff
InitVDCTab:
		.byte VDC_DSP_HI, 0		; viewpage 0 as default
		.byte VDC_DSP_LO, 0
		.byte VDC_HSCROLL, $87
		.byte 2, $66
		.byte 4, $4c
		.byte 5, $06
		.byte 6, $4c
		.byte 7, $47
		.byte 8, $03
		.byte 9, $06
		.byte 27, $00
		.byte $ff

SCN80CLR:	.byte 27,88,147,27,88,0

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
;
; Must set an error code: NO
;

INSTALL:
	; check for VDC version and update register $19 value

	; check for VDC ram size and update number of available screens

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
	lda	#1
	sta	pages
@endok:
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
	lda	pages			; is there enough memory?
	bne	@L11			; Jump if there is one screen
	lda	#TGI_ERR_INV_MODE	; Error
	bne	@L9

; Initialize variables

@L11:   ldx     #$FF
        stx     BITMASK

; Remeber current color value
	ldx	#VDC_COLORS
	jsr	VDCReadReg
	sta	OLDCOLOR

; Switch into graphics mode (set view page 0)

	ldy	#0
@L2:	ldx	InitVDCTab,y
	bmi	@L3
	iny
	lda	InitVDCTab,y
	jsr	VDCWriteReg
	iny
	bne	@L2
@L3:

; Done, reset the error code

        lda     #TGI_ERR_OK
@L9:    sta     ERROR
        rts

; ------------------------------------------------------------------------
; DONE: Will be called to switch the graphics device back into text mode.
; The graphics kernel will never call DONE when no graphics mode is active,
; so there is no need to protect against that.
;
; Must set an error code: NO
;

DONE:
	; This part is C128-mode specific
	jsr $e179		; reload character set and setup VDC
	jsr $ff62
	lda $d7			; in 80-columns?
	bne @L01
@L0:	lda SCN80CLR,y
	beq @L1
	jsr $ffd2		; print \xe,clr,\xe
	iny
	bne @L0
@L01:	lda #147
	jsr $ffd2		; print clr
@L1:	lda #0			; restore view page
	ldx #VDC_DSP_HI
	jsr VDCWriteReg
	lda OLDCOLOR
	ldx #VDC_COLORS
	jsr VDCWriteReg		; restore color (background)
	lda #$47
	ldx #VDC_HSCROLL
       	jmp VDCWriteReg		; switch to text screen

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
	lda	#0
	tay
	jsr	VDCSetSourceAddr
	lda	#0
	ldx	#VDC_VSCROLL
	jsr	VDCWriteReg			; set fill mode
	lda	#0
	jsr	VDCWriteByte			; put 1rst byte (fill value)
	ldy	#159				; 159 times
	lda	#0				; 256 bytes
	ldx	#VDC_COUNT
@L1:	jsr	VDCWriteReg
	dey
	bne	@L1
	rts

; ------------------------------------------------------------------------
; SETVIEWPAGE: Set the visible page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;

SETVIEWPAGE:
	rts

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

SETCOLOR:
        tax
        beq     @L1
        lda     #$FF
@L1:    sta     BITMASK
        rts

; ------------------------------------------------------------------------
; SETPALETTE: Set the palette (not available with all drivers/hardware).
; A pointer to the palette is passed in ptr1. Must set an error if palettes
; are not supported
;
; Must set an error code: YES
;

SETPALETTE:
        ldy     #PALETTESIZE - 1
@L1:    lda     (ptr1),y        ; Copy the palette
        and     #$0F            ; Make a valid color
        sta     PALETTE,y
        dey
        bpl     @L1

; Get the color entries from the palette

        ldy     PALETTE+1       ; Foreground color
	lda	COLTRANS,y
        asl     a
        asl     a
        asl     a
        asl     a
        ldy     PALETTE         ; Background color
	ora	COLTRANS,y

	ldx	#VDC_COLORS
       	jsr     VDCWriteReg
        lda     #TGI_ERR_OK     ; Clear error code
        sta     ERROR
        rts

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
	lda	xres
	ldx	xres+1
	sta	ADDR
	stx	ADDR+1
	ldx	#ADDR
	lda	X1
	ldy	X1+1
	jsr	icmp		; if (xres<x1)
	bcs	@cont		; !(xres<x1)
@finito:rts
@cont:	lda	yres
	ldx	yres+1
	sta	ADDR
	stx	ADDR+1
	ldx	#ADDR
	lda	Y1
	ldy	Y1+1
	jsr	icmp		; if (yres<y1)
	bcc	@finito

SETPIXEL:
        jsr     CALC            ; Calculate coordinates

	stx	TEMP
	lda	ADDR
	ldy	ADDR+1
	jsr	VDCSetSourceAddr
	jsr	VDCReadByte
	ldx	TEMP

	sta	TEMP
        eor     BITMASK
        and     BITTAB,X
	eor	TEMP
	pha
	lda	ADDR
	ldy	ADDR+1
	jsr	VDCSetSourceAddr
	pla
	jsr	VDCWriteByte

@L9:    rts

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.


GETPIXEL:
        jsr     CALC            ; Calculate coordinates

	stx	TEMP		; preserve X
	lda	ADDR
	ldy	ADDR+1
	jsr	VDCSetSourceAddr
	jsr	VDCReadByte
	ldx	TEMP

        ldy     #$00
        and     BITTAB,X
        beq     @L1
        iny

@L1:    tya                     ; Get color value into A
        ldx     #$00            ; Clear high byte
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
	sec
	sbc	X1
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
	sec
	sbc	Y1
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
	; dx = 1;
	lda	#1
	bne	@L0244
	; else
	; dx = -1;
@L0243:	lda	#$ff
@L0244:	sta	DX
	; if (y2>y1)
	ldx	#Y2
	lda	Y1
	ldy	Y1+1
	jsr	icmp
	bcc	@L024A
	; dy = 1;
	lda	#1
	bne	@L024B
	; else
	; dy = -1;
@L024A:	lda	#$ff
@L024B:	sta	DY
	; err = ay = 0;
	lda	#0
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
	lda	#0
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
	clc
	adc	NY
	sta	PB
	lda	ERR+1
	adc	NY+1
	sta	PB+1
	tax
	;    ub = pb + nx
	lda	PB
	clc
	adc	NX
	sta	UB
	txa
	adc	NX+1
	sta	UB+1
	;    x1 = x1 + dx
	ldx	#0
	lda	DX
	bpl	@L027B
	dex
@L027B:	clc
	adc	X1
	sta	X1
	txa
	adc	X1+1
	sta	X1+1
	;   y1 = y1 + ay
	ldx	#0
	lda	AY
	bpl	@L027E
	dex
@L027E:	clc
	adc	Y1
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
@L0288:	clc
	adc	X1
	sta	X1
	txa
	adc	X1+1
	sta	X1+1
	;	y1 = y1 + dy
	ldx	#0
	lda	DY
	bpl	@L028B
	dex
@L028B:	clc
	adc	Y1
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
@L0260:	dec	COUNT+1
	jmp	@L0166

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
	inc	Y2
	bne	HORLINE
	inc	Y2+1

; Original code for a horizontal line

HORLINE:
	lda X1
	pha
	lda X1+1
	pha
	jsr CALC		; get data for LEFT
	lda BITMASKL,x		; remember left address and bitmask
	pha
	lda ADDR
	pha
	lda ADDR+1
	pha

	lda X2
	sta X1
	lda X2+1
	sta X1+1
	jsr CALC		; get data for RIGHT
	lda BITMASKR,x
	sta TEMP3

	pla			; recall data for LEFT
	sta X1+1
	pla
	sta X1			; put left address into X1
	pla

	cmp #%11111111		; if left bit <> 0
	beq @L1
	sta TEMP2		; do left byte only...
	lda X1
	ldy X1+1
	jsr VDCSetSourceAddr
	jsr VDCReadByte
	sta TEMP
	eor BITMASK
	and TEMP2
	eor TEMP
	pha
	lda X1
	ldy X1+1
	jsr VDCSetSourceAddr
	pla
	jsr VDCWriteByte
	inc X1			; ... and proceed
	bne @L1
	inc X1+1

	; do right byte (if Y2=0 ++ADDR and skip)
@L1:	lda TEMP3
	cmp #%11111111		; if right bit <> 7
	bne @L11
	inc ADDR		; right bit = 7 - the next one is the last
	bne @L10
	inc ADDR+1
@L10:	bne @L2

@L11:	lda ADDR		; do right byte only...
	ldy ADDR+1
	jsr VDCSetSourceAddr
	jsr VDCReadByte
	sta TEMP
	eor BITMASK
	and TEMP3
	eor TEMP
	pha
	lda ADDR
	ldy ADDR+1
	jsr VDCSetSourceAddr
	pla
	jsr VDCWriteByte

@L2:				; do the fill in the middle
	lda ADDR		; calculate offset in full bytes
	sec
	sbc X1
	beq @L3			; if equal - there are no more bytes
	sta ADDR

	lda X1			; setup for the left side
	ldy X1+1
	jsr VDCSetSourceAddr
	lda BITMASK		; get color
	jsr VDCWriteByte	; put 1st value
	ldx ADDR
	dex
	beq @L3			; 1 byte already written

	stx ADDR		; if there are more bytes - fill them...
	ldx #VDC_VSCROLL
	lda #0
	jsr VDCWriteReg		; setup for fill
	ldx #VDC_COUNT
	lda ADDR
	jsr VDCWriteReg		; ... fill them NOW!

@L3:	pla
	sta X1+1
	pla
	sta X1

; End of horizontal line code

	inc	Y1
	bne	@L4
	inc	Y1+1
@L4:	lda	Y1
	cmp	Y2
	bne	@L5
	lda	Y1+1
	cmp	Y2+1
	bne	@L5
	rts

@L5:	jmp	HORLINE


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
        rts

; ------------------------------------------------------------------------
; Calculate all variables to plot the pixel at X1/Y1.
;------------------------
;< X1,Y1 - pixel
;> ADDR - address of card
;> X - bit number (X1 & 7)
CALC:
	lda	Y1
	pha
	lda	Y1+1
	pha
	lsr
	ror	Y1		; Y=Y/2
	sta	Y1+1
	sta	ADDR+1
	lda	Y1
	asl
	rol	ADDR+1
	asl
	rol	ADDR+1		; Y*4
	clc
	adc	Y1
	sta	ADDR
	lda	Y1+1
	adc	ADDR+1
	sta	ADDR+1		; Y*4+Y=Y*5
	lda	ADDR
	asl
	rol	ADDR+1
	asl
	rol	ADDR+1
	asl
	rol	ADDR+1
	asl
	rol	ADDR+1
	sta	ADDR		; Y*5*16=Y*80
	lda	X1+1
	sta	TEMP
	lda	X1
	lsr	TEMP
	ror
	lsr	TEMP
	ror
	lsr	TEMP
	ror
	clc
	adc	ADDR
	sta	ADDR
	lda	ADDR+1		; ADDR = Y*80+x/8
	adc	TEMP
	sta	ADDR+1
	pla
	sta	Y1+1
	pla
	sta	Y1
	and	#1
	beq	@even		; even line - no offset
	lda	ADDR
	clc
	adc	#<21360
	sta	ADDR
	lda	ADDR+1
	adc	#>21360
	sta	ADDR+1		; odd lines are 21360 bytes farther
@even:	lda	X1
	and	#7
	tax
	rts

;-------------
; copies of some runtime routines

abs:
	; a/y := abs(a/y)
	dey
	iny
	bpl	absend
	; negay
neg:	clc
	eor	#$ff
	adc	#1
	pha
	tya
	eor	#$ff
	adc	#0
	tay
	pla
absend:	rts

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

