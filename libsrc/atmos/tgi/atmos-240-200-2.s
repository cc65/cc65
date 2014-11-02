;
; Graphics driver for the 240x200x2 monochrome mode on the Atmos
;
; Stefan Haubenthal <polluks@sdf.lonestar.org>
; 2014-09-10, Greg King <gregdk@users.sf.net>
;

        .include        "zeropage.inc"

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"
        .include        "atmos.inc"

        .macpack        generic
        .macpack        module

XSIZE   =       6                       ; System font width
YSIZE   =       8                       ; System font height

; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

        module_header   _atmos_240_200_2_tgi

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

        .byte   $74, $67, $69           ; "tgi"
        .byte   TGI_API_VERSION         ; TGI API version number
        .addr   $0000                   ; Library reference
        .word   240                     ; X resolution
        .word   200                     ; Y resolution
        .byte   2                       ; Number of drawing colors
        .byte   1                       ; Number of screens available
        .byte   XSIZE                   ; System font X size
        .byte   YSIZE                   ; System font Y size
        .word   $011C                   ; Aspect ratio (based on 4/3 display)
        .byte   0                       ; TGI driver flags

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
        .addr   TEXTSTYLE
        .addr   OUTTEXT
        .addr   0                       ; IRQ entry is unused

; ------------------------------------------------------------------------
; Data.

; Variables mapped to the zero page segment variables. Some of these are
; used for passing parameters to the driver.

X1              := ptr1
Y1              := ptr2
X2              := ptr3
Y2              := ptr4

; Absolute variables used in the code

.bss

ERROR:          .res    1       ; Error code
MODE:           .res    1       ; Graphics mode

; Constant table

.rodata

DEFPALETTE:     .byte   0, 1

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

        jsr     HIRES

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

DONE            := TEXT

; ------------------------------------------------------------------------
; GETERROR: Return the error code in A and clear it.

GETERROR:
        ldx     #TGI_ERR_OK
        lda     ERROR
        stx     ERROR
        rts

; ------------------------------------------------------------------------
; CONTROL: Platform/driver specific entry point.
;
; Must set an error code: YES
;

CONTROL:
        sta     PATTERN
        lda     #TGI_ERR_OK
        sta     ERROR
        rts

; ------------------------------------------------------------------------
; CLEAR: Clears the screen.
;
; Must set an error code: NO
;

CLEAR           := HIRES

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

SETCOLOR:
        sta     MODE
        rts

; ------------------------------------------------------------------------
; SETPALETTE: Set the palette (not available with all drivers/hardware).
; A pointer to the palette is passed in ptr1. Must set an error if palettes
; are not supported
;
; Must set an error code: YES
;

SETPALETTE:
        lda     #TGI_ERR_INV_FUNC       ; This resolution has no palette
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

SETPIXEL:
        lda     Y1
        sta     PARAM2
        lda     MODE
mymode: sta     PARAM3
        lda     X1
        sta     PARAM1
        lda     #>$0000
        sta     PARAM1+1
        sta     PARAM2+1
        sta     PARAM3+1
        jmp     CURSET

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.

GETPIXEL:
        lda     X1
        sta     PARAM1
        lda     Y1
        sta     PARAM2
        lda     #>$0000
        sta     PARAM1+1
        sta     PARAM2+1
        jsr     POINT
        lda     PARAM1
        and     #%00000001
        ldx     #>$0000
        rts

; ------------------------------------------------------------------------
; LINE: Draw a line from X1/Y1 to X2/Y2, where X1/Y1 = ptr1/ptr2 and
; X2/Y2 = ptr3/ptr4 using the current drawing color.
;
; Must set an error code: NO
;

LINE:
        jsr     SETPIXEL
        lda     X2
        sub     X1
        sta     PARAM1
        lda     X2+1
        sbc     X1+1
        sta     PARAM1+1
        lda     Y2
        sub     Y1
        sta     PARAM2
        lda     Y2+1
        sbc     Y1+1
        sta     PARAM2+1
        lda     MODE
        sta     PARAM3
        ldx     #>$0000
        stx     PARAM3+1
        jmp     DRAW

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
        inc     Y2
@L1:    lda     Y2
        pha
        lda     Y1
        sta     Y2
        jsr     LINE
        pla
        sta     Y2
        inc     Y1
        cmp     Y1
        bne     @L1
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
        lda     Y1
        sub     #(YSIZE - 1)
        sta     PARAM2
        lda     #3              ; (Move graphics cursor; don't draw)
        jsr     mymode

        ldy     #0
@next:  lda     (ptr3),y
        beq     @end
        sta     PARAM1
        lda     #0
        sta     PARAM2
        sta     PARAM1+1
        sta     PARAM2+1
        sta     PARAM3+1
        lda     MODE
        sta     PARAM3
        tya
        pha
        jsr     CHAR
        lda     #XSIZE
        sta     PARAM1
        lda     #0
        sta     PARAM2
        sta     PARAM1+1
        sta     PARAM2+1
        sta     PARAM3+1
        lda     #3
        sta     PARAM3
        jsr     CURMOV
        pla
        tay
        iny
        bne     @next
@end:   rts
