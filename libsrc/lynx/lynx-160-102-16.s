;
; Graphics driver for the 160x102x16 mode on the Lynx.
;
; Based on Stephen L. Judds GRLIB code
;

	.include 	"zeropage.inc"

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
        .byte   1                       ; Number of screens available
        .byte   8                       ; System font X size
        .byte   8                       ; System font Y size
        .res    4, $00                  ; Reserved for future extensions

; Next comes the jump table. Currently all entries must be valid and may point
; to an RTS for test versions (function not implemented). A future version may
; allow for emulation: In this case the vector will be zero. Emulation means
; that the graphics kernel will emulate the function by using lower level
; primitives - for example ploting a line by using calls to SETPIXEL.

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

X1              := ptr1
Y1              := ptr2
X2              := ptr3
Y2              := ptr4
RADIUS          := tmp1

ROW             := tmp2         ; Bitmap row...
COL             := tmp3         ; ...and column, both set by PLOT
TEMP            := tmp4
TEMP2           := sreg
POINT           := regsave
INRANGE         := regsave+2    ; PLOT variable, $00 = coordinates in range

CHUNK           := X2           ; Used in the line routine
OLDCHUNK        := X2+1         ; Dito

; Absolute variables used in the code

.bss

ERROR:  	.res	1     	; Error code

DRAWINDEX:      .res    1	; Pen to use for drawing
VIEWPAGEL:	.res	1
VIEWPAGEH:	.res	1

; INIT/DONE
OLDD018:        .res    1       ; Old register value

; Line routine stuff
DX:             .res    2
DY:             .res    2

; Circle routine stuff, overlaid by BAR variables
X1SAVE:
CURX:           .res    1
CURY:           .res    1
Y1SAVE:
BROW:           .res    1       ; Bottom row
TROW:           .res    1       ; Top row
X2SAVE:
LCOL:           .res    1       ; Left column
RCOL:           .res    1       ; Right column
Y2SAVE:
CHUNK1:         .res    1
OLDCH1:         .res    1
CHUNK2:         .res    1
OLDCH2:         .res    1

; Text output stuff
TEXTMAGX:       .res    1
TEXTMAGY:       .res    1
TEXTDIR:        .res    1

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

BITTAB:         .byte   $80,$40,$20,$10,$08,$04,$02,$01
BITCHUNK:       .byte   $FF,$7F,$3F,$1F,$0F,$07,$03,$01

VBASE  	       	= $E000         ; Video memory base address
CBASE           = $D000         ; Color memory base address


.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
;
; Must set an error code: NO
;

INSTALL:
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

CONTROL:
	lda	#TGI_ERR_INV_FUNC
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
draw_sprite:
       	sta     $fc10
       	stx     $fc11
       	lda     #1
       	sta     $fc91
       	stz     $fd90
@L3:    stz     CPUSLEEP
       	lda     $fc92
       	lsr
       	bcs     @L3
       	stz     $fd90
	rts

; ------------------------------------------------------------------------
; SETVIEWPAGE: Set the visible page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;

SETVIEWPAGE:
       	beq     @L1
       	lda     #<$fe00-8160-8160
       	ldx     #>$fe00-8160-8160
       	bra     @L2
@L1:    lda     #<$fe00-8160
       	ldx     #>$fe00-8160
@L2:    sta     DISPADRL            ; $FD94
       	sta     VIEWPAGEL
       	stx     DISPADRH            ; $FD95
       	sta     VIEWPAGEH
        rts

; ------------------------------------------------------------------------
; SETDRAWPAGE: Set the drawable page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;

SETDRAWPAGE:
       	beq     @L1
       	lda     #<$fe00-8160-8160
       	ldx     #>$fe00-8160-8160
       	bra     @L2
@L1:    lda     #<$fe00-8160
       	ldx     #>$fe00-8160
@L2:    sta     VIDBASL            ; $FD94
       	stx     VIDBASH            ; $FD95
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
       	.word   $0100
       	.word   $0100
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
       	lda     (ptr1),y
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
; To deal with off-screen coordinates, the current row
; and column (40x25) is kept track of.  These are set
; negative when the point is off the screen, and made
; positive when the point is within the visible screen.
;
; X1,X2 etc. are set up above (x2=LINNUM in particular)
; Format is LINE x2,y2,x1,y1
;
; Must set an error code: NO
;

LINE:
        rts

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
       	sta     bar_sx+1
       	lda     Y2
       	sec
       	sbc     Y1
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
        rts

