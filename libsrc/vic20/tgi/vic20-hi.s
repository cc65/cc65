;
; Graphics driver for a 160x192x2 mode on the VIC-20.
;
; Based on C64 TGI
;
; 2018-03-11, Sven Michael Klose
;

        .include        "zeropage.inc"

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .macpack        generic
        .macpack        module


XRES := 160
YRES := 192

; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

        module_header   _vic20_hi_tgi

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

        .byte   $74, $67, $69           ; "tgi"
        .byte   TGI_API_VERSION         ; TGI API version number
        .addr   $0000                   ; Library reference
        .word   XRES                    ; X resolution
        .word   YRES                    ; Y resolution
        .byte   2                       ; Number of drawing colors
        .byte   1                       ; Number of screens available
        .byte   8                       ; System font X size
        .byte   8                       ; System font Y size
        .word   $00D4                   ; Aspect ratio (based on 4/3 display)
        .byte   0                       ; TGI driver flags

; Next comes the jump table. With the exception of IRQ, all entries must be
; valid and may point to an RTS for test versions (function not implemented).

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   INIT
        .addr   $e518           ; KERNAL VIC init.
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

; ------------------------------------------------------------------------
; Data.

; Variables mapped to the zero page segment variables. Some of these are
; used for passing parameters to the driver.

X1              := ptr1
Y1              := ptr2
X2              := ptr3
Y2              := ptr4
TEXT            := ptr3

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

ERROR:          .res    1       ; Error code
PALETTE:        .res    2       ; The current palette

BITMASK:        .res    1       ; $00 = clear, $FF = set pixels

; INIT/DONE
OLDD018:        .res    1       ; Old register value

; Line routine stuff
DX:             .res    2
DY:             .res    2

; BAR variables
X1SAVE:         .res    2
Y1SAVE:         .res    2
X2SAVE:         .res    2
Y2SAVE:         .res    2

; Text output stuff
TEXTMAGX:       .res    1
TEXTMAGY:       .res    1
TEXTDIR:        .res    1

; Constants and tables

.rodata

DEFPALETTE:     .byte   $00, $01        ; White on black
PALETTESIZE     = * - DEFPALETTE

BITTAB:         .byte   $80,$40,$20,$10,$08,$04,$02,$01
BITCHUNK:       .byte   $FF,$7F,$3F,$1F,$0F,$07,$03,$01

CHARROM         := $8000                ; Character ROM base address
CBASE           := $9400                ; Color memory base address
SBASE           := $1000                ; Screen memory base address
VBASE           := $1100                ; Video memory base address

VICREGS:
        .byte $02
        .byte $fe
        .byte $fe
        .byte $eb
        .byte $00
        .byte $0c

XADDRS_L:
        .byte <(VBASE + YRES * 0)
        .byte <(VBASE + YRES * 1)
        .byte <(VBASE + YRES * 2)
        .byte <(VBASE + YRES * 3)
        .byte <(VBASE + YRES * 4)
        .byte <(VBASE + YRES * 5)
        .byte <(VBASE + YRES * 6)
        .byte <(VBASE + YRES * 7)
        .byte <(VBASE + YRES * 8)
        .byte <(VBASE + YRES * 9)
        .byte <(VBASE + YRES * 10)
        .byte <(VBASE + YRES * 11)
        .byte <(VBASE + YRES * 12)
        .byte <(VBASE + YRES * 13)
        .byte <(VBASE + YRES * 14)
        .byte <(VBASE + YRES * 15)
        .byte <(VBASE + YRES * 16)
        .byte <(VBASE + YRES * 17)
        .byte <(VBASE + YRES * 18)
        .byte <(VBASE + YRES * 19)

XADDRS_H:
        .byte >(VBASE + YRES * 0)
        .byte >(VBASE + YRES * 1)
        .byte >(VBASE + YRES * 2)
        .byte >(VBASE + YRES * 3)
        .byte >(VBASE + YRES * 4)
        .byte >(VBASE + YRES * 5)
        .byte >(VBASE + YRES * 6)
        .byte >(VBASE + YRES * 7)
        .byte >(VBASE + YRES * 8)
        .byte >(VBASE + YRES * 9)
        .byte >(VBASE + YRES * 10)
        .byte >(VBASE + YRES * 11)
        .byte >(VBASE + YRES * 12)
        .byte >(VBASE + YRES * 13)
        .byte >(VBASE + YRES * 14)
        .byte >(VBASE + YRES * 15)
        .byte >(VBASE + YRES * 16)
        .byte >(VBASE + YRES * 17)
        .byte >(VBASE + YRES * 18)
        .byte >(VBASE + YRES * 19)

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

; Initialize variables

        ldx     #$FF
        stx     BITMASK

; Make screen columns.
        lda     #<SBASE
        sta     tmp2
        lda     #>SBASE
        sta     tmp2+1
        ldx     #0
l:      ldy     #0
        txa
        clc
        adc     #$10
m:      sta     (tmp2),y
        clc
        adc     #12
        iny
        cpy     #20
        bne     m
        lda     tmp2
        clc
        adc     #20
        sta     tmp2
        bcc     n
        inc     tmp2+1
n:      inx
        cpx     #12
        bne     l

; Set up VIC.
        ldx #5
l2:     clc
        lda     $ede4,x
        adc     VICREGS,x
        sta     $9000,x
        dex
        bpl     l2
        lda     $900f
        and     #%00000111
        ora     #8
        sta     $900f

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

;DONE:   jmp    $e518       ; KERNAL VIC init.

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
        lda     #TGI_ERR_INV_FUNC
        sta     ERROR
        rts

; ------------------------------------------------------------------------
; CLEAR: Clears the screen.
;
; Must set an error code: NO
;

CLEAR:  ldy     #$00
@L1:    lda     #$00
        sta     $1100,y
        sta     $1200,y
        sta     $1300,y
        sta     $1400,y
        sta     $1500,y
        sta     $1600,y
        sta     $1700,y
        sta     $1800,y
        sta     $1900,y
        sta     $1a00,y
        sta     $1b00,y
        sta     $1c00,y
        sta     $1d00,y
        sta     $1e00,y
        sta     $1f00,y
        lda     #1
        sta     $9400,y
        sta     $9500,y
        iny
        bne     @L1
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

; Initialize the color map with the new color settings.

        ldy     #$00
        lda     PALETTE+1       ; Foreground color
@L2:    sta     CBASE+$0000,y
        sta     CBASE+$0100,y
        iny
        bne     @L2
        lda     $900f
        and     #$f8
        ora     PALETTE         ; Background color
        sta     $900f

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

SETPIXEL:
        jsr     CALC            ; Calculate coordinates

        ldy     #$00
        lda     (POINT),Y
        eor     BITMASK
        and     BITTAB,X
        eor     (POINT),Y
        sta     (POINT),Y

        rts

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.


GETPIXEL:
        jsr     CALC            ; Calculate coordinates

        lda     X1
        and     #$07
        tax
        lda     (POINT),Y
        ldy     #$00
        and     BITTAB,X
        beq     @L1
        iny

@L1:    tya                     ; Get color value into A
        ldx     #$00            ; Clear high byte
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

; Note: This function needs optimization. It's just a cheap translation of
; the original C wrapper and could be written much smaller (besides that,
; calling LINE is not a good idea either).

BAR:    lda     Y2
        sta     Y2SAVE
        lda     Y2+1
        sta     Y2SAVE+1

        lda     X2
        sta     X2SAVE
        lda     X2+1
        sta     X2SAVE+1

        lda     Y1
        sta     Y1SAVE
        lda     Y1+1
        sta     Y1SAVE+1

        lda     X1
        sta     X1SAVE
        lda     X1+1
        sta     X1SAVE+1

@L1:    lda     Y1
        sta     Y2
        lda     Y1+1
        sta     Y2+1
        jsr     LINE

        lda     Y1SAVE
        cmp     Y2SAVE
        bne     @L2
        lda     Y1SAVE
        cmp     Y2SAVE
        beq     @L4

@L2:    inc     Y1SAVE
        bne     @L3
        inc     Y1SAVE+1

@L3:    lda     Y1SAVE
        sta     Y1
        lda     Y1SAVE+1
        sta     Y1+1

        lda     X1SAVE
        sta     X1
        lda     X1SAVE+1
        sta     X1+1

        lda     X2SAVE
        sta     X2
        lda     X2SAVE+1
        sta     X2+1
        jmp     @L1

@L4:    rts


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

; Calculate a pointer to the representation of the character in the
; character ROM

        ldx     #((>(CHARROM + $0800)) >> 3)
        ldy     #0
        lda     (TEXT),y
        bmi     @L1
        ldx     #((>(CHARROM + $0000)) >> 3)
@L1:    stx     ptr4+1
        asl     a
        rol     ptr4+1
        asl     a
        rol     ptr4+1
        asl     a
        rol     ptr4+1
        sta     ptr4





        rts

; ------------------------------------------------------------------------
; Calculate all variables to plot the pixel at X1/Y1. If the point is out
; of range, a carry is returned and INRANGE is set to a value != zero. If
; the coordinates are valid, INRANGE is zero and the carry clear.

CALC:
        lda     X1+1
        bne     @L9
        lda     Y1+1
        bne     @L9

        lda     Y1
        cmp     #YRES
        bcs     @L9
        sta     ROW

        lda     X1
        cmp     #XRES
        bcs     @L9
        lsr
        lsr
        lsr
        tay

        lda     XADDRS_L,y
        clc
        adc     Y1
        sta     POINT
        lda     XADDRS_H,y
        adc     #0
        sta     POINT+1

        lda     X1
        and     #7
        tax

        lda     #0
        sta     INRANGE
        clc
        rts

@L9:    lda     #1
        sta     INRANGE
        sec
        rts

.include "../../tgi/tgidrv_line.inc"
