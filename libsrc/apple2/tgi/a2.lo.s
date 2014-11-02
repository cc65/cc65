;
; Graphics driver for the 40x48x16 mode on the Apple II
;
; Stefan Haubenthal <polluks@sdf.lonestar.org>
; Oliver Schmidt <ol.sc@web.de>
;

        .include        "zeropage.inc"

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"
        .include        "apple2.inc"

        .macpack        module

; ------------------------------------------------------------------------

; Zero page stuff

H2      :=      $2C
COLOR   :=      $30

; ROM entry points

TEXT    :=      $F399
PLOT    :=      $F800
HLINE   :=      $F819
CLRSC2  :=      $F838
SETCOL  :=      $F864
SCRN    :=      $F871
SETGR   :=      $FB40
HOME    :=      $FC58

; Used for passing parameters to the driver

X1      :=      ptr1
Y1      :=      ptr2
X2      :=      ptr3
Y2      :=      ptr4

; ------------------------------------------------------------------------

        .ifdef  __APPLE2ENH__
        module_header   _a2e_lo_tgi
        .else
        module_header   _a2_lo_tgi
        .endif

; Header. Includes jump table and constants.

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

        .byte   $74, $67, $69   ; "tgi"
        .byte   TGI_API_VERSION ; TGI API version number
        .addr   $0000           ; Library reference
        .word   40              ; X resolution
        .word   48              ; Y resolution
        .byte   16              ; Number of drawing colors
        .byte   1               ; Number of screens available
        .byte   8               ; System font X size
        .byte   8               ; System font Y size
        .word   $0198           ; Aspect ratio (based on 4/3 display)
        .byte   0               ; TGI driver flags

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

ERROR:  .res    1               ; Error code
MIX:    .res    1               ; 4 lines of text

; ------------------------------------------------------------------------

        .rodata

DEFPALETTE: .byte $00, $01, $02, $03, $04, $05, $06, $07
            .byte $08, $09, $0A, $0B, $0C, $0D, $0E, $0F

TGI2COL:    .byte $00, $0C, $03, $0F, $01, $09, $06, $02
            .byte $04, $05, $07, $08, $0A, $0B, $0D, $0E

COL2TGI:    .byte $00, $04, $07, $02, $08, $09, $06, $0A
            .byte $0B, $05, $0C, $0D, $01, $0E, $0F, $03

MAXY:   .byte 47, 39

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
        bit     $C082           ; Switch in ROM
        jsr     SETGR
        bit     MIXCLR
        bit     $C080           ; Switch in LC bank 2 for R/O

        ; Done, reset the error code
        lda     #TGI_ERR_OK
        sta     ERROR
        sta     MIX

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
        bit     $C082           ; Switch in ROM
        jsr     TEXT
        jsr     HOME
        bit     $C080           ; Switch in LC bank 2 for R/O
        rts

; GETERROR: Return the error code in A and clear it.
GETERROR:
        lda     ERROR
        ldx     #TGI_ERR_OK
        stx     ERROR
        rts

; CLEAR: Clears the screen.
; Must set an error code: NO
CLEAR:
        bit     $C082           ; Switch in ROM
        lda     COLOR           ; Save current drawing color
        pha
        ldx     MIX
        ldy     MAXY,x          ; Max Y depends on 4 lines of text
        jsr     CLRSC2
        pla
        sta     COLOR           ; Restore current drawing color
        bit     $C080           ; Switch in LC bank 2 for R/O
        rts

; SETCOLOR: Set the drawing color (in A). The new color is already checked
; to be in a valid range (0..maxcolor-1).
; Must set an error code: NO (will only be called if color ok)
SETCOLOR:
        bit     $C082           ; Switch in ROM
        tax
        lda     TGI2COL,x
        jsr     SETCOL
        bit     $C080           ; Switch in LC bank 2 for R/O
        rts

; CONTROL: Platform/driver specific entry point.
; Must set an error code: YES
CONTROL:
        ; Check data msb and code to be 0
        ora     ptr1+1
        bne     err

        ; Check data lsb to be [0..1]
        lda     ptr1
        cmp     #1+1
        bcs     err
        bit     $C082           ; Switch in ROM

        ; Switch 4 lines of text
        tax
        .assert MIXCLR + 1 = MIXSET, error
        lda     MIXCLR,x        ; No BIT absolute,X available

        ; Save current switch setting
        txa
        sta     MIX
        bne     text

        ; Clear 8 lines of graphics
        lda     COLOR           ; Save current drawing color
        pha
        lda     #39             ; Rightmost column
        sta     H2
        ldx     #40             ; First line
:       txa
        ldy     #$00            ; Leftmost column
        sty     COLOR           ; Black
        jsr     HLINE           ; Preserves X
        inx
        cpx     #47+1           ; Last line
        bcc     :-
        pla
        sta     COLOR           ; Restore current drawing color
        bcs     :+              ; Branch always

        ; Clear 4 lines of text
text:   jsr     HOME
:       bit     $C080           ; Switch in LC bank 2 for R/O

        ; Done, reset the error code
        lda     #TGI_ERR_OK
        beq     :+              ; Branch always

        ; Done, set the error code
err:    lda     #TGI_ERR_INV_ARG
:       sta     ERROR
        rts

; SETPALETTE: Set the palette (not available with all drivers/hardware).
; A pointer to the palette is passed in ptr1. Must set an error if palettes
; are not supported
; Must set an error code: YES
SETPALETTE:
        lda     #TGI_ERR_INV_FUNC
        sta     ERROR
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
        lda     #<DEFPALETTE
        ldx     #>DEFPALETTE
        rts

; SETPIXEL: Draw one pixel at X1/Y1 = ptr1/ptr2 with the current drawing
; color. The coordinates passed to this function are never outside the
; visible screen area, so there is no need for clipping inside this function.
; Must set an error code: NO
SETPIXEL:
        bit     $C082           ; Switch in ROM
        ldy     X1
        lda     Y1
        jsr     PLOT
        bit     $C080           ; Switch in LC bank 2 for R/O
        rts

; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.
GETPIXEL:
        bit     $C082           ; Switch in ROM
        ldy     X1
        lda     Y1
        jsr     SCRN
        tax
        lda     COL2TGI,x
        ldx     #$00
        bit     $C080           ; Switch in LC bank 2 for R/O
        rts

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
; Must set an error code: NO
BAR:
        bit     $C082           ; Switch in ROM
        lda     X2
        sta     H2
        inc     Y2
        ldx     Y1
:       txa
        ldy     X1
        jsr     HLINE           ; Preserves X
        inx
        cpx     Y2
        bcc     :-
        bit     $C080           ; Switch in LC bank 2 for R/O
        rts

; ------------------------------------------------------------------------

.include        "../../tgi/tgidrv_line.inc"
