;
; Graphics driver for the 160x102x16 mode on the Lynx.
;
; All the drawing functions are simply done by sprites as the sprite
; engine is the only way to do fast graphics on a Lynx.
;
; So the code is not really based on any algorithms done by somebody.
; I have looked at other routines in the cc65 libs to see what kind of
; entry points we need. And I have looked at the old cc65 libs by
; Bastian Schick to see how things worked in the past.
;
; This code is written by Karri Kaksonen, 2004 for the cc65 compiler.
;

	.include 	"zeropage.inc"
	.include 	"extzp.inc"

      	.include 	"tgi-kernel.inc"
        .include        "tgi-mode.inc"
        .include        "tgi-error.inc"

	.include        "lynx.inc"

        .macpack        generic

; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

.segment        "JUMPTABLE"

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

        .byte   $74, $67, $69           ; "tgi"
        .byte   TGI_API_VERSION         ; TGI API version number
        .word   160                     ; X resolution
        .word   102                     ; Y resolution
        .byte   16                      ; Number of drawing colors
        .byte   2                       ; Number of screens available
        .byte   8                       ; System font X size
        .byte   8                       ; System font Y size
        .res    4, $00                  ; Reserved for future extensions

; Next comes the jump table. Currently all entries must be valid and may point
; to an RTS for test versions (function not implemented). A future version may
; allow for emulation: In this case the vector will be zero. Emulation means
; that the graphics kernel will emulate the function by using lower level
; primitives - for example ploting a line by using calls to SETPIXEL.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   INIT
        .addr   DONE
        .addr	GETERROR
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
        .addr   IRQ


; ------------------------------------------------------------------------
; Data.

; Variables mapped to the zero page segment variables. Some of these are
; used for passing parameters to the driver.

X1              := ptr1
Y1              := ptr2
X2              := ptr3
Y2              := ptr4

STRPTR		:= ptr3
FONTOFF		:= ptr4
STROFF		:= tmp3
STRLEN		:= tmp4

; Absolute variables used in the code

.bss

ERROR:  	.res	1     	; Error code

DRAWINDEX:      .res    1	; Pen to use for drawing
VIEWPAGEL:	.res	1
VIEWPAGEH:	.res	1
DRAWPAGEL:	.res	1
DRAWPAGEH:	.res	1

; Text output stuff
TEXTMAGX:       .res    1
TEXTMAGY:       .res    1
TEXTDIR:        .res    1
BGINDEX:        .res    1	; Pen to use for text background

; Double buffer IRQ stuff
DRAWPAGE:	.res	1
SWAPREQUEST:	.res	1
VBLHOOK:	.res    3

text_bitmap:    .res	8*(1+20+1)+1
; 8 rows with (one offset-byte plus 20 character bytes plus one fill-byte) plus one 0-offset-byte

; Constants and tables

.rodata

DEFPALETTE:     .byte   >$000
		.byte	>$007
		.byte	>$070
		.byte	>$700
		.byte	>$077
		.byte	>$770
		.byte	>$707
		.byte	>$777
		.byte	>$333
		.byte	>$00F
		.byte	>$0F0
		.byte	>$F00
		.byte	>$0FF
		.byte	>$FF0
		.byte	>$F0F
		.byte	>$FFF
		.byte   <$000
		.byte	<$007
		.byte	<$070
		.byte	<$700
		.byte	<$077
		.byte	<$770
		.byte	<$707
		.byte	<$777
		.byte	<$333
		.byte	<$00F
		.byte	<$0F0
		.byte	<$F00
		.byte	<$0FF
		.byte	<$FF0
		.byte	<$F0F
		.byte	<$FFF

PALETTESIZE     = * - DEFPALETTE

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
;
; Must set an error code: NO
;

INSTALL:
	lda	#1
	sta	TEXTMAGX
	sta	TEXTMAGY
	stz	BGINDEX
	stz	DRAWPAGE
	stz	SWAPREQUEST
	lda	#$60		; rts op-code
	sta	VBLHOOK
        rts


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
        rts

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
; The TGI lacks a way to draw sprites. As that functionality is vital to
; Lynx games we borrow this CONTROL function to implement the still
; missing tgi_draw_sprite funtion. To use this in your C-program
; do a #define tgi_draw_sprite(spr) tgi_ioctl(0, spr)
;
; To do a flip-screen call tgi_ioctl(1, 0)
;
; To set the background index for text outputs call tgi_ioctl(2, bgindex)
;
; To set the frame rate for the display hardware call tgi_ioctl(3, rate)
;
; To check if the drawing engine is busy with the previous swap you can
; call tgi_ioctl(4, 0). It returns 0 if idle and 1 if busy
;
; To update displays you can call tgi_ioctl(4, 1) it will wait for the
; next VBL interrupt and swap draw and view buffers.
;
; Set an address for a subroutine you want to call at every VBL
; tgi_ioctl(5, hook)

CONTROL:
        pha			; Almost all control routines succeed
        lda     #TGI_ERR_OK
	sta	ERROR
	pla
	cmp	#5
	bne ControlSwapRequest

	lda	ptr1		; Set IRQ routine to be called at VBL
	sta	VBLHOOK+1
	lda	ptr1+1
	sta	VBLHOOK+2
	lda	#$43		; jmp op-code
	sta	VBLHOOK
	rts

ControlSwapRequest:
	cmp	#4
	bne	ControlFramerate

	lda	ptr1		; Swap request
	bne	@L0
	lda	SWAPREQUEST
	rts
@L0:    sta	SWAPREQUEST
	rts

ControlFramerate:
	cmp	#3
	bne	ControlTextBG

       	cmp     #75		; Set framerate
       	beq     rate75
       	cmp     #60
       	beq     rate60
        cmp     #50
       	beq     rate50
        lda     #TGI_ERR_INV_ARG
	sta	ERROR
        rts
rate50: lda     #$bd		; 50 Hz
       	ldx     #$31
       	bra     setRate
rate60: lda     #$9e		; 60 Hz
       	ldx     #$29
       	bra     setRate
rate75: lda     #$7e		; 75 Hz
       	ldx     #$20
setRate:
        sta     HTIMBKUP
       	stx     PBKUP
	rts

ControlTextBG:
	cmp	#2
	bne	ControlFlipScreen

	lda	ptr1		; Set text background color
	sta	BGINDEX
	rts

ControlFlipScreen:
	cmp	#1
	bne	ControlDrawSprite

	lda	__sprsys	; Flip screen
	eor	#8
	sta	__sprsys
	sta	SPRSYS
	lda	__viddma
	eor	#2
	sta	__viddma
	sta	DISPCTL
	ldy	VIEWPAGEL
	ldx	VIEWPAGEH
	and	#2
	beq	NotFlipped
	clc
	tya
	adc	#<8159
	tay
	txa
	adc	#>8159
	tax
NotFlipped:
	sty	DISPADRL
	stx	DISPADRH
        rts

ControlDrawSprite:
	lda	ptr1		; Get the sprite address
	ldx	ptr1+1

draw_sprite:			; Draw it in render buffer
       	sta     SCBNEXTL
       	stx     SCBNEXTH
	lda	DRAWPAGEL
	ldx	DRAWPAGEH
	sta     VIDBASL
       	stx     VIDBASH
       	lda     #1
       	sta     SPRGO
       	stz     SDONEACK
@L0:    stz     CPUSLEEP
       	lda     SPRSYS
       	lsr
       	bcs     @L0
       	stz     SDONEACK
        lda     #TGI_ERR_OK
	sta	ERROR
        rts

; ------------------------------------------------------------------------
; CLEAR: Clears the screen.
;
; Must set an error code: NO
;

.rodata
pixel_bitmap:
        .byte   3,%10000100,%00000000, $0       ; A pixel bitmap
cls_sprite:
        .byte   %00000001			; A pixel sprite
       	.byte   %00010000
       	.byte   %00100000
       	.addr   0,pixel_bitmap
       	.word   0
       	.word   0
       	.word   $a000  	                        ; 160
       	.word   $6600	                        ; 102
       	.byte   $00

.code
CLEAR:  lda     #<cls_sprite
       	ldx     #>cls_sprite
	bra	draw_sprite

; ------------------------------------------------------------------------
; SETVIEWPAGE: Set the visible page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;
; It is a good idea to call this function during the vertical blanking
; period. If you call it in the middle of the screen update then half of
; the drawn frame will be from the old buffer and the other half is
; from the new buffer. This is usually noticed by the user.

SETVIEWPAGE:
	cmp	#1
       	beq     @L1             ; page == maxpages-1
       	ldy     #<$de20         ; page 0
       	ldx     #>$de20
       	bra     @L2
@L1:
	ldy     #<$be40         ; page 1
       	ldx     #>$be40
@L2:
       	sty     VIEWPAGEL	; Save viewpage for getpixel
       	stx     VIEWPAGEH

	lda	__viddma	; Process flipped displays
	and	#2
	beq	@L3
	clc
	tya
	adc	#<8159
	tay
	txa
	adc	#>8159
	tax
@L3:
        sty     DISPADRL        ; $FD94
       	stx     DISPADRH        ; $FD95
        rts

; ------------------------------------------------------------------------
; SETDRAWPAGE: Set the drawable page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;

SETDRAWPAGE:
	cmp	#1
       	beq     @L1                 ; page == maxpages-1
       	lda     #<$de20             ; page 0
       	ldx     #>$de20
       	bra     @L2
@L1:
        lda     #<$be40             ; page 1
       	ldx     #>$be40
@L2:
        sta     DRAWPAGEL
       	stx     DRAWPAGEH
        rts

; ------------------------------------------------------------------------
; IRQ: VBL interrupt handler
;

TIMER0_INTERRUPT = $01
TIMER1_INTERRUPT = $02
TIMER2_INTERRUPT = $04
TIMER3_INTERRUPT = $08
TIMER4_INTERRUPT = $10
TIMER5_INTERRUPT = $20
TIMER6_INTERRUPT = $40
TIMER7_INTERRUPT = $80

HBL_INTERRUPT = TIMER0_INTERRUPT
VBL_INTERRUPT = TIMER2_INTERRUPT
SERIAL_INTERRUPT = TIMER4_INTERRUPT

IRQ:
	lda	INTSET		; Poll all pending interrupts
	and	#VBL_INTERRUPT
        bne	IRQVBL
	clc			; Not a VBL interrupt - exit
	rts
IRQVBL: 
	lda	SWAPREQUEST
	beq	@L0
	lda	DRAWPAGE
	jsr	SETVIEWPAGE
	lda	DRAWPAGE
	eor	#1
	sta	DRAWPAGE
	jsr	SETDRAWPAGE
	stz	SWAPREQUEST
@L0:
	jsr	VBLHOOK
	clc
	rts

; ------------------------------------------------------------------------
; SETCOLOR: Set the drawing color (in A). The new color is already checked
; to be in a valid range (0..maxcolor-1).
;
; Must set an error code: NO (will only be called if color ok)
;

SETCOLOR:
        sta     DRAWINDEX
        rts

; ------------------------------------------------------------------------
; SETPALETTE: Set the palette (not available with all drivers/hardware).
; A pointer to the palette is passed in ptr1. Must set an error if palettes
; are not supported
;
; Must set an error code: YES
;

SETPALETTE:
       	ldy     #31
@L1:    lda     (ptr1),y
       	sta     GCOLMAP,y   ; $FDA0
	dey
       	bpl     @L1

; Done, reset the error code

        lda     #TGI_ERR_OK
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
       	lda     #<GCOLMAP	; $FDA0
       	ldx     #>GCOLMAP
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

.data
pixel_sprite:
        .byte   %00000001			; A pixel sprite
       	.byte   %00010000
       	.byte   %00100000
       	.addr   0,pixel_bitmap
pix_x: 	.word   0
pix_y: 	.word   0
	.word   $100
	.word   $100
pix_c: 	.byte   $00

.code
SETPIXEL:
        lda     X1
        sta     pix_x
        lda     Y1
        sta     pix_y
       	lda     DRAWINDEX
       	sta     pix_c
        lda     #<pixel_sprite
       	ldx     #>pixel_sprite
       	jmp     draw_sprite

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.


GETPIXEL:
        lda     Y1
       	sta     MATHD	; Hardware multiply
       	stz     MATHC
       	lda     #80
       	sta     MATHB
       	stz     MATHA
       	lda     X1
       	lsr     A
	php
	tay

	clc
       	lda     VIEWPAGEL
       	adc     MATHH
       	sta     ptr1
       	lda     VIEWPAGEH
       	adc     MATHG
       	sta     ptr1+1

       	ldx     #0
	lda	#15
	sta	MAPCTL
       	lda     (ptr1),y
	tay
	lda	#$0c
	sta	MAPCTL
	tya
       	plp
       	bcc     @L1
       	and     #$f
	rts

@L1:    lsr     A
       	lsr     A
       	lsr     A
       	lsr     A
	rts

; ------------------------------------------------------------------------
; LINE: Draw a line from X1/Y1 to X2/Y2, where X1/Y1 = ptr1/ptr2 and
; X2/Y2 = ptr3/ptr4 using the current drawing color.
;
; Must set an error code: NO
;

.data
line_sprite:
	.byte   0		; Will be replaced by the code
	.byte	%00110000
	.byte	%00100000
	.word   0,pixel_bitmap
line_x:
	.word    0
line_y:
	.word    0
line_sx:
	.word    $100
line_sy:
	.word    $100
	.word    0
line_tilt:
	.word    0
line_c:
	.byte    $e

.code
LINE:
	lda	DRAWINDEX
	sta	line_c
	stz	line_sx
	stz	line_sy

	sec
	lda	X2
	sbc	X1
	lda	X2+1
	sbc	X1+1
	bpl	@L1
	lda	X1
	ldx	X2
	sta	X2
	stx	X1
	lda	X1+1
	ldx	X2+1
	sta	X2+1
	stx	X1+1
	lda	Y1
	ldx	Y2
	sta	Y2
     	stx	Y1
	lda	Y1+1
	ldx	Y2+1
	sta	Y2+1
	stx	Y1+1
@L1:
	lda	#%00000000	; Not flipped
	sta	line_sprite

	sec
	lda	Y2
	sbc	Y1
	sta	Y2
	lda	Y2+1
	sbc	Y1+1
	sta	Y2+1
	bpl	@L2
	sec
	lda	#0
	sbc	Y2
	sta	Y2
	lda	#0
	sbc	Y2+1
	sta	Y2+1
     	lda	#%00010000	; Vertical flip
	sta	line_sprite
@L2:
	lda	X1
	sta	line_x
	lda	X1+1
	sta	line_x+1
	lda	Y1
	sta	line_y
	lda	Y1+1
	sta	line_y+1

	lda	Y2
	ina
	sta	line_sy+1
	sta	MATHP		; hardware divide
	stz	MATHN

	stz	MATHH
	stz	MATHG
	sec
	lda	X2
	sbc	X1
	ina
     	sta	MATHF
	stz	MATHE
@L3:
	lda	SPRSYS
	bmi	@L3		; wait for math done (bit 7 of sprsys)
	lda	MATHC
	sta	line_tilt
	lda	MATHB
	sta	line_tilt+1
	bne	@L4
	lda	#1
	sta	line_sx+1
	bra	@L6
@L4:
	bit	line_tilt
	bpl	@L5
	ina
@L5:
	sta	line_sx+1
@L6:
	lda	#<line_sprite
	ldx	#>line_sprite
	jmp	draw_sprite

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

.data
bar_sprite:
        .byte   %00000001			; A pixel sprite
     	.byte   %00010000
     	.byte   %00100000
     	.addr   0,pixel_bitmap
bar_x:	.word   0
bar_y:	.word   0
bar_sx:	.word   $0100
bar_sy:	.word   $0100
bar_c:	.byte   $00

.code
BAR:    lda     X1
        sta     bar_x
        lda     Y1
        sta     bar_y
       	lda     X2
       	sec
       	sbc     X1
     	ina
       	sta     bar_sx+1
       	lda     Y2
       	sec
       	sbc     Y1
	ina
       	sta     bar_sy+1
       	lda     DRAWINDEX
       	sta     bar_c
        lda     #<bar_sprite
       	ldx     #>bar_sprite
       	jmp     draw_sprite

; ------------------------------------------------------------------------
; CIRCLE: Draw a circle around the center X1/Y1 (= ptr1/ptr2) with the
; radius in tmp1 and the current drawing color.
;
; Must set an error code: NO
;
; There is no sensible way of drawing a circle on a Lynx. As I would
; have to use line elements to do the circle I rather do it in C than
; create it here in the driver.

; To do a circle please add this to your C program

;int sintbl[9] = {
;    	0,     //  0    degrees
;	3196,  // 11.25 degrees
;	6270,  // 22.5  degrees
;	9102,  // 33.75 degrees
;	11585, // 45    degrees
;	13623, // 56.25 degrees
;	15137, // 67.5  degrees
;	16069, // 78.75 degrees
;	16384  // 90    degrees
;};

;int sin(char d)
;{
;    char neg;
;    d = d & 31;
;    neg = d > 16;
;    d = d & 15;
;    if (d > 8)
;        d = 16 - d;
;    if (neg)
;	return -sintbl[d];
;    else
;	return sintbl[d];
;}

;void tgi_Circle(int x0, int y0, unsigned char r)
;{
;	char i;
;	int x1, y1, x2, y2;
;
;	x1 = ((long)sin(0) * r + 8192) / 16384 + x0;
;	y1 = ((long)sin(8) * r + 8192) / 16384 + y0;
;	for (i = 1; i <= 32; i++) {
;		x2 = ((long)sin(i) * r + 8192) / 16384 + x0;
;		y2 = ((long)sin(i+8) * r + 8192) / 16384 + y0;
;		tgi_line(x1, y1, x2, y2);
;		x1 = x2;
;		y1 = y2;
;	}
;}

;#define tgi_circle tgi_Circle

CIRCLE:
	rts

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
	lda	TEXTMAGX	; Scale sprite
	sta	text_sx+1
	lda	TEXTMAGY
	sta	text_sy+1

	stz	text_sprite	; Set normal sprite
	lda	BGINDEX
	bne	@L1
	lda	#4
	sta	text_sprite	; Set opaque sprite
@L1:
	lda	DRAWINDEX	; Set color
	asl
	asl
	asl
	asl
	ora	BGINDEX
	sta	text_c

	lda	X1		; Set start position
	sta	text_x
	lda	X1+1
	sta	text_x+1
	lda	Y1
	sta	text_y
	lda	Y1+1
	sta	text_y+1

     	ldy	#-1		; Calculate string length
@L2:
	iny
	lda	(STRPTR),y
	bne	@L2
	cpy	#20
	bmi	@L3
	ldy	#20
@L3:
	sty	STRLEN
        bne	@L4
        rts			; Zero length string
@L4:
	iny			; Prepare text_bitmap
        iny
        sty	STROFF

        ldy	#8-1        	; 8 pixel lines per character
        ldx	#0
        clc
@L5:
        lda	STROFF
        sta	text_bitmap,x
        txa
        adc	STROFF
        tax
        lda	#$ff
        sta	text_bitmap-1,x
        dey
        bpl	@L5
        stz	text_bitmap,x

        stz	tmp2
        iny
@L6:
        lda	(STRPTR),y
        sty	tmp1

        sec			; (ch-' ') * 8
        sbc	#32
        stz	FONTOFF
        stz	FONTOFF+1
        asl
        asl
        rol	FONTOFF+1
        asl
        rol	FONTOFF+1
        clc			; Choose font
        adc	#<font
        sta	FONTOFF
        lda	FONTOFF+1
        adc	#>font
        sta	FONTOFF+1

; and now copy the 8 bytes of that char

        ldx	tmp2
        inx
        stx	tmp2

; draw char from top to bottom, reading char-data from offset 8-1 to offset 0
        ldy	#8-1
@L7:
        lda	(FONTOFF),y         ; *chptr
        sta	text_bitmap,x    ;textbuf[y*(1+len+1)+1+x]

        txa
        adc	STROFF
        tax

        dey
        bpl	@L7

        ; goto next char
        ldy	tmp1
        iny
        dec	STRLEN
        bne	@L6

        lda	#<text_sprite
        ldx	#>text_sprite
        jmp	draw_sprite

.data
text_sprite:
        .byte $00,$90,$20
        .addr 0, text_bitmap
text_x:
        .word 0
text_y:
        .word 0
text_sx:
        .word $100
text_sy:
        .word $100
text_c:
        .byte 0

.rodata
; The Font
; 96 characters from ASCII 32 to 127
; 8 pixels wide, 8 pixels high
; bit value 0 = foreground, bit value 1 = background / transparent 
font:
; VERSAIL
     	.byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF  ;32
     	.byte $FF, $E7, $FF, $FF, $E7, $E7, $E7, $E7  ;33
     	.byte $FF, $FF, $FF, $FF, $FF, $99, $99, $99  ;34
     	.byte $FF, $99, $99, $00, $99, $00, $99, $99  ;35
     	.byte $FF, $E7, $83, $F9, $C3, $9F, $C1, $E7  ;36
     	.byte $FF, $B9, $99, $CF, $E7, $F3, $99, $9D  ;37
     	.byte $FF, $C0, $99, $98, $C7, $C3, $99, $C3  ;38
     	.byte $FF, $FF, $FF, $FF, $FF, $E7, $F3, $F9  ;39
     	.byte $FF, $F3, $E7, $CF, $CF, $CF, $E7, $F3  ;40
     	.byte $FF, $CF, $E7, $F3, $F3, $F3, $E7, $CF  ;41
     	.byte $FF, $FF, $99, $C3, $00, $C3, $99, $FF  ;42
     	.byte $FF, $FF, $E7, $E7, $81, $E7, $E7, $FF  ;43
     	.byte $CF, $E7, $E7, $FF, $FF, $FF, $FF, $FF  ;44
     	.byte $FF, $FF, $FF, $FF, $81, $FF, $FF, $FF  ;45
     	.byte $FF, $E7, $E7, $FF, $FF, $FF, $FF, $FF  ;46
     	.byte $FF, $9F, $CF, $E7, $F3, $F9, $FC, $FF  ;47
	.byte $FF, $C3, $99, $99, $89, $91, $99, $C3  ;48
	.byte $FF, $81, $E7, $E7, $E7, $C7, $E7, $E7  ;49
	.byte $FF, $81, $9F, $CF, $F3, $F9, $99, $C3  ;50
	.byte $FF, $C3, $99, $F9, $E3, $F9, $99, $C3  ;51
	.byte $FF, $F9, $F9, $80, $99, $E1, $F1, $F9  ;52
	.byte $FF, $C3, $99, $F9, $F9, $83, $9F, $81  ;53
	.byte $FF, $C3, $99, $99, $83, $9F, $99, $C3  ;54
	.byte $FF, $E7, $E7, $E7, $E7, $F3, $99, $81  ;55
	.byte $FF, $C3, $99, $99, $C3, $99, $99, $C3  ;56
	.byte $FF, $C3, $99, $F9, $C1, $99, $99, $C3  ;57
	.byte $FF, $FF, $E7, $FF, $FF, $E7, $FF, $FF  ;58
	.byte $CF, $E7, $E7, $FF, $FF, $E7, $FF, $FF  ;59
	.byte $FF, $F1, $E7, $CF, $9F, $CF, $E7, $F1  ;60
	.byte $FF, $FF, $FF, $81, $FF, $81, $FF, $FF  ;61
	.byte $FF, $8F, $E7, $F3, $F9, $F3, $E7, $8F  ;62
	.byte $FF, $E7, $FF, $E7, $F3, $F9, $99, $C3  ;63


	.byte $FF, $C3, $9D, $9F, $91, $91, $99, $C3  ;0
	.byte $FF, $99, $99, $99, $81, $99, $C3, $E7  ;1
	.byte $FF, $83, $99, $99, $83, $99, $99, $83  ;2
	.byte $FF, $C3, $99, $9F, $9F, $9F, $99, $C3  ;3
	.byte $FF, $87, $93, $99, $99, $99, $93, $87  ;4
     	.byte $FF, $81, $9F, $9F, $87, $9F, $9F, $81  ;5
	.byte $FF, $9F, $9F, $9F, $87, $9F, $9F, $81  ;6
	.byte $FF, $C3, $99, $99, $91, $9F, $99, $C3  ;7
	.byte $FF, $99, $99, $99, $81, $99, $99, $99  ;8
	.byte $FF, $C3, $E7, $E7, $E7, $E7, $E7, $C3  ;9
	.byte $FF, $C7, $93, $F3, $F3, $F3, $F3, $E1  ;10
	.byte $FF, $99, $93, $87, $8F, $87, $93, $99  ;11
	.byte $FF, $81, $9F, $9F, $9F, $9F, $9F, $9F  ;12
	.byte $FF, $9C, $9C, $9C, $94, $80, $88, $9C  ;13
	.byte $FF, $99, $99, $91, $81, $81, $89, $99  ;14
	.byte $FF, $C3, $99, $99, $99, $99, $99, $C3  ;15
	.byte $FF, $9F, $9F, $9F, $83, $99, $99, $83  ;16
	.byte $FF, $F1, $C3, $99, $99, $99, $99, $C3  ;17
	.byte $FF, $99, $93, $87, $83, $99, $99, $83  ;18
	.byte $FF, $C3, $99, $F9, $C3, $9F, $99, $C3  ;19
	.byte $FF, $E7, $E7, $E7, $E7, $E7, $E7, $81  ;20
	.byte $FF, $C3, $99, $99, $99, $99, $99, $99  ;21
	.byte $FF, $E7, $C3, $99, $99, $99, $99, $99  ;22
	.byte $FF, $9C, $88, $80, $94, $9C, $9C, $9C  ;23
	.byte $FF, $99, $99, $C3, $E7, $C3, $99, $99  ;24
	.byte $FF, $E7, $E7, $E7, $C3, $99, $99, $99  ;25
	.byte $FF, $81, $9F, $CF, $E7, $F3, $F9, $81  ;26
	.byte $FF, $C3, $CF, $CF, $CF, $CF, $CF, $C3  ;27
	.byte $FF, $03, $9D, $CF, $83, $CF, $ED, $F3  ;28
	.byte $FF, $C3, $F3, $F3, $F3, $F3, $F3, $C3  ;29
	.byte $E7, $E7, $E7, $E7, $81, $C3, $E7, $FF  ;30
	.byte $FF, $EF, $CF, $80, $80, $CF, $EF, $FF  ;31


; gemena
	.byte $FF, $C3, $9D, $9F, $91, $91, $99, $C3  ;224
	.byte $FF, $C1, $99, $C1, $F9, $C3, $FF, $FF  ;225
	.byte $FF, $83, $99, $99, $83, $9F, $9F, $FF  ;226
	.byte $FF, $C3, $9F, $9F, $9F, $C3, $FF, $FF  ;227
	.byte $FF, $C1, $99, $99, $C1, $F9, $F9, $FF  ;228
	.byte $FF, $C3, $9F, $81, $99, $C3, $FF, $FF  ;229
	.byte $FF, $E7, $E7, $E7, $C1, $E7, $F1, $FF  ;230
	.byte $83, $F9, $C1, $99, $99, $C1, $FF, $FF  ;231
	.byte $FF, $99, $99, $99, $83, $9F, $9F, $FF  ;232
	.byte $FF, $C3, $E7, $E7, $C7, $FF, $E7, $FF  ;233
	.byte $C3, $F9, $F9, $F9, $F9, $FF, $F9, $FF  ;234
	.byte $FF, $99, $93, $87, $93, $9F, $9F, $FF  ;235
	.byte $FF, $C3, $E7, $E7, $E7, $E7, $C7, $FF  ;236
	.byte $FF, $9C, $94, $80, $80, $99, $FF, $FF  ;237
	.byte $FF, $99, $99, $99, $99, $83, $FF, $FF  ;238
	.byte $FF, $C3, $99, $99, $99, $C3, $FF, $FF  ;239
	.byte $9F, $9F, $83, $99, $99, $83, $FF, $FF  ;240
	.byte $F9, $F9, $C1, $99, $99, $C1, $FF, $FF  ;241
	.byte $FF, $9F, $9F, $9F, $99, $83, $FF, $FF  ;242
	.byte $FF, $83, $F9, $C3, $9F, $C1, $FF, $FF  ;243
	.byte $FF, $F1, $E7, $E7, $E7, $81, $E7, $FF  ;244
	.byte $FF, $C1, $99, $99, $99, $99, $FF, $FF  ;245
	.byte $FF, $E7, $C3, $99, $99, $99, $FF, $FF  ;246
	.byte $FF, $C9, $C1, $80, $94, $9C, $FF, $FF  ;247
	.byte $FF, $99, $C3, $E7, $C3, $99, $FF, $FF  ;248
	.byte $87, $F3, $C1, $99, $99, $99, $FF, $FF  ;249
	.byte $FF, $81, $CF, $E7, $F3, $81, $FF, $FF  ;250
	.byte $FF, $C3, $CF, $CF, $CF, $CF, $CF, $C3  ;251
	.byte $FF, $03, $9D, $CF, $83, $CF, $ED, $F3  ;252
	.byte $FF, $C3, $F3, $F3, $F3, $F3, $F3, $C3  ;253
	.byte $E7, $E7, $E7, $E7, $81, $C3, $E7, $FF  ;254
	.byte $FF, $EF, $CF, $80, $80, $CF, $EF, $FF  ;255
