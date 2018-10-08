;
; Graphics driver for a max. 160x50x2 (but 4 pixels) mode on all CBMs.
;
; 2018-08-30, Stefan Haubenthal
; 2018-09-14, Greg King
;

        .include        "zeropage.inc"

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"
        .include        "pet.inc"       ; SCR_LINELEN
        .ifndef DYN_DRV
        .include        "cbm_kernal.inc"
        .else
;        .ifndef __PLUS4__
        .import         PLOT, BSOUT
;        .endif
        .ifndef SCREEN
        .import         SCREEN
        .endif
        .endif

        .macpack        generic
        .macpack        module

; ------------------------------------------------------------------------
; Constants

.if .def(__CBM510__) || .def(__CBM610__) || .def(__C128__) || .def(__C16__)
fix             = 2
.else
fix             = 0
.endif

RVS_ON          = 18
RVS_OFF         = 18+128

; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

        module_header   MODULE_LABEL

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

        .byte   $74, $67, $69           ; "tgi"
        .byte   TGI_API_VERSION         ; TGI API version number
libref: .addr   $0000                   ; Library reference
xres:   .word   80 - fix                ; X resolution (Plus/4 default)
yres:   .word   50                      ; Y resolution
        .byte   2                       ; Number of drawing colors
        .byte   1                       ; Number of screens available
        .byte   2                       ; System font X size
        .byte   2                       ; System font Y size
        .word   $00D4                   ; Aspect ratio (based on 4/3 display)
        .byte   0                       ; TGI driver flags

; Next comes the jump table. Currently all entries must be valid and may point
; to an RTS for test versions (function not implemented).

        .word   INSTALL
        .word   UNINSTALL
        .word   INIT
        .word   DONE
        .word   GETERROR
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

TEMP            := tmp1
TEMP3           := tmp3

; Absolute variables used in the code

.bss

COPY:           .res    160/2*50/2
xcells:         .res    1       ; Number of physical cells across the screen
ERROR:          .res    1       ; Error code
COLOR:          .res    1       ; Current color

; Constants and tables

.rodata

DEFPALETTE:     .byte   $0, $1

;                       84
;                       21
OFFSET:         .byte   8, 4, 2, 1

;                       00   00   00   00   01   01       01       01
;                       00   01   10   11   00   01       10       11
CODE:           .byte   160, 172, 187, 162, 188, 161-128, 191-128, 190-128
;                       10   10   10   10       11       11       11       11
;                       00   01   10   11       00       01       10       11
                .byte   190, 191, 161, 188-128, 162-128, 187-128, 172-128, 160-128

.ifdef __PLUS4__
.data

my_PLOT:
                .byte   $4C, 0, 0
my_BSOUT:
                .byte   $4C, 0, 0
.endif

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
;
; Must set an error code: NO
;

INSTALL:
        .ifndef __PLUS4__
        .ifdef __PET__
        ldx     SCR_LINELEN
        inx                     ; Variable is one less
        .else
        jsr     SCREEN
        sty     yres
        asl     yres            ; yres*2
        .endif
        stx     xcells
        .if fix
        dex
        .endif
        stx     xres
        asl     xres            ; xres*2
        .else
        lda     libref
        sta     my_PLOT+1
        lda     libref+1
        sta     my_PLOT+2
        .endif
;       rts

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
;       rts

; ------------------------------------------------------------------------
; DONE: Will be called to switch the graphics device back into text mode.
; The graphics kernel will never call DONE when no graphics mode is active,
; so there is no need to protect against that.
;
; Must set an error code: NO
;

;DONE:

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

CLEAR:
        lda     #0
        tay
@L1:    sta     COPY+$0000,y
        sta     COPY+$0100,y
        sta     COPY+$0200,y
        sta     COPY+$0300,y
        sta     COPY+$0400,y
        sta     COPY+$0500,y
        sta     COPY+$0600,y
        sta     COPY+$0652,y
        iny
        bne     @L1
DONE:
        lda     #147
        jmp     BSOUT

; ------------------------------------------------------------------------
; SETCOLOR: Set the drawing color (in A). The new color is already checked
; to be in a valid range (0..maxcolor-1).
;
; Must set an error code: NO (will only be called if color ok)
;

SETCOLOR:
        sta     COLOR
;       rts

; ------------------------------------------------------------------------
; SETVIEWPAGE: Set the visible page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will only be called if page ok)
;

SETVIEWPAGE:
;       rts

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
        lda     #TGI_ERR_INV_FUNC
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
;       rts

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
        .if fix = 0
        ldx     Y1              ; y+2<yres
        inx
        inx
        cpx     yres
        bcc     @L2
        ldx     X1              ; x+2<xres
        inx
        inx
        cpx     xres
        bcc     @L2
        rts
        .endif
@L2:    lda     X1
        lsr
        tay
        lda     Y1
        lsr
        tax
        .ifdef __PET__
        stx     198
        sty     216
        .else
        clc
        jsr     PLOT
        .endif
        jsr     calc
        ldx     COLOR
        bne     @set2
        eor     #%00001111
@set2:  sta     TEMP3

        lda     (TEMP),y
        ldx     COLOR
        bne     @set
        and     TEMP3
        .byte   $2c
@set:   ora     TEMP3
        sta     (TEMP),y
        tax
        lda     CODE,x
        bmi     @normal
        pha
        lda     #RVS_ON
        jsr     BSOUT
        pla
        ora     #128
        jsr     BSOUT
        lda     #RVS_OFF
@normal:jmp     BSOUT

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.

GETPIXEL:
        jsr     calc
        sta     TEMP3
        lda     (TEMP),y
        and     TEMP3
        beq     @L1
        lda     #1
@L1:    ldx     #0
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

BAR:
        inc     Y2
@L1:    lda     X1
        pha
@L2:    jsr     SETPIXEL
        inc     X1
        lda     X2
        cmp     X1
        bne     @L2
        pla
        sta     X1
        inc     Y1
        lda     Y2
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
        lda     ptr1
        lsr
        tay
        lda     ptr2
        lsr
        tax
        .ifdef __PET__
        stx     198
        sty     216
        .else
        clc
        jsr     PLOT
        .endif
        ldy     #0
@L1:    lda     (ptr3),y
        jsr     BSOUT
        iny
        cmp     #$0
        bne     @L1
        rts

; ------------------------------------------------------------------------
; Calculate all variables to plot the pixel at (X1,Y1).
;
; xcells * Y1/2 + COPY + X1/2

calc:   lda     Y1
        lsr     a
        tay
        lda     #$00
        tax
        beq     @L2

@L1:    add     xcells
        bcc     @L2
        inx
@L2:    dey
        bpl     @L1

        add     #<COPY
        sta     TEMP
        txa
        adc     #>COPY
        sta     TEMP+1          ; Set cell row

        lda     X1
        lsr     a
        tay                     ; Set cell column
        lda     Y1
        rol     a
        and     #%00000011
        tax
        lda     OFFSET,x        ; Get bitmask within cell
        rts

; ------------------------------------------------------------------------

.include        "../tgi/tgidrv_line.inc"
