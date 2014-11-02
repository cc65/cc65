;
; Graphics driver for the 280x192x8 mode on the Apple II
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

HBASL   :=      $26
HMASK   :=      $30
PAGE    :=      $E6
SCALE   :=      $E7
ROT     :=      $F9

; Graphics entry points, by cbmnut (applenut??) cbmnut@hushmail.com

TEXT    :=      $F399   ; Return to text screen
HGR2    :=      $F3D8   ; Initialize and clear hi-res page 2.
HGR     :=      $F3E2   ; Initialize and clear hi-res page 1.
HCLR    :=      $F3F2   ; Clear the current hi-res screen to black.
BKGND   :=      $F3F6   ; Clear the current hi-res screen to the
                        ; last plotted color (from ($1C).
HPOSN   :=      $F411   ; Positions the hi-res cursor without
                        ; plotting a point.
                        ; Enter with (A) = Y-coordinate, and
                        ; (Y,X) = X-coordinate.
HPLOT   :=      $F457   ; Calls HPOSN and tries to plot a dot at
                        ; the cursor's position.  If you are
                        ; trying to plot a non-white color at
                        ; a complementary color position, no
                        ; dot will be plotted.
HLIN    :=      $F53A   ; Draws a line from the last plotted
                        ; point or line destination to:
                        ; (X,A) = X-coordinate, and
                        ; (Y) = Y-coordinate.
HFIND   :=      $F5CB   ; Converts the hi-res coursor's position
                        ; back to X- and Y-coordinates; stores
                        ; X-coordinate at $E0,E1 and Y-coordinate
                        ; at $E2.
DRAW    :=      $F601   ; Draws a shape.  Enter with (Y,X) = the
                        ; address of the shape table, and (A) =
                        ; the rotation factor.  Uses the current
                        ; color.
XDRAW   :=      $F65D   ; Draws a shape by inverting the existing
                        ; color of the dots the shape draws over.
                        ; Same entry parameters as DRAW.
SETHCOL :=      $F6EC   ; Set the hi-res color to (X), where (X)
                        ; must be between 0 and 7.

; ------------------------------------------------------------------------

; Variables mapped to the zero page segment variables. Some of these are
; used for passing parameters to the driver.

X1      :=      ptr1
Y1      :=      ptr2
X2      :=      ptr3
Y2      :=      ptr4

; ------------------------------------------------------------------------

        .ifdef  __APPLE2ENH__
        module_header   _a2e_hi_tgi
        .else
        module_header   _a2_hi_tgi
        .endif

; Header. Includes jump table and constants.

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

        .byte   $74, $67, $69   ; "tgi"
        .byte   TGI_API_VERSION ; TGI API version number
        .addr   $0000           ; Library reference
        .word   280             ; X resolution
        .word   192             ; Y resolution
        .byte   8               ; Number of drawing colors
pages:  .byte   2               ; Number of screens available
        .byte   7               ; System font X size
        .byte   8               ; System font Y size
        .word   $00EA           ; Aspect ratio (based on 4/3 display)
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

; Absolute variables used in the code

ERROR:  .res    1               ; Error code

; ------------------------------------------------------------------------

        .rodata

; Constants and tables

DEFPALETTE: .byte $00, $01, $02, $03, $04, $05, $06, $07

FONT:
        ; Beagle Bros Shape Mechanic font F.ASCII.SMALL
        ; modified to exactly reproduce the text glyphs
        .incbin "a2.hi.fnt"

; ------------------------------------------------------------------------

        .code

; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
; Must set an error code: NO
INSTALL:
        .ifdef  __APPLE2ENH__
        ; No page switching if 80 column store is enabled
        bit     RD80COL
        bpl     :+
        lda     #$01
        sta     pages
:       .endif

        ; Fall through

; UNINSTALL routine. Is called before the driver is removed from memory. May
; clean up anything done by INSTALL but is probably empty most of the time.
; Must set an error code: NO
UNINSTALL:
        rts

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
        bit     MIXCLR
        bit     HIRES
        bit     TXTCLR

        ; Beagle Bros Shape Mechanic fonts don't
        ; scale well so use fixed scaling factor
        lda     #$01
        sta     SCALE

        ; Done, reset the error code
        lda     #TGI_ERR_OK
        sta     ERROR
        rts

; DONE: Will be called to switch the graphics device back into text mode.
; The graphics kernel will never call DONE when no graphics mode is active,
; so there is no need to protect against that.
; Must set an error code: NO
DONE:
        ; Switch into text mode
        bit     TXTSET
        bit     LOWSCR

        .ifdef  __APPLE2ENH__
        ; Limit SET80COL-HISCR to text
        bit     LORES
        .endif

        ; Reset the text window top
        lda     #$00
        sta     WNDTOP
        rts

; GETERROR: Return the error code in A and clear it.
GETERROR:
        lda     ERROR
        ldx     #TGI_ERR_OK
        stx     ERROR
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

        ; Set text window top
        tax
        beq     :+
        lda     #20
:       sta     WNDTOP

        ; Switch 4 lines of text
        .assert MIXCLR + 1 = MIXSET, error
        lda     MIXCLR,x        ; No BIT absolute,X available

        ; Done, reset the error code
        lda     #TGI_ERR_OK
        beq     :+              ; Branch always

        ; Done, set the error code
err:    lda     #TGI_ERR_INV_ARG
:       sta     ERROR
        rts

; CLEAR: Clears the screen.
; Must set an error code: NO
CLEAR:
        bit     $C082           ; Switch in ROM
        jsr     HCLR
        bit     $C080           ; Switch in LC bank 2 for R/O
        rts

; SETVIEWPAGE: Set the visible page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
; Must set an error code: NO (will only be called if page ok)
SETVIEWPAGE:
        tax
        .assert LOWSCR + 1 = HISCR, error
        lda     LOWSCR,x        ; No BIT absolute,X available
        rts

; SETDRAWPAGE: Set the drawable page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
; Must set an error code: NO (will only be called if page ok)
SETDRAWPAGE:
        tax
        beq     :+
        lda     #>$4000         ; Page 2
        .byte   $2C             ; BIT absolute
:       lda     #>$2000         ; Page 1
        sta     PAGE
        rts

; SETCOLOR: Set the drawing color (in A). The new color is already checked
; to be in a valid range (0..maxcolor-1).
; Must set an error code: NO (will only be called if color ok)
SETCOLOR:
        bit     $C082           ; Switch in ROM
        tax
        jsr     SETHCOL
        bit     $C080           ; Switch in LC bank 2 for R/O
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
        ldx     X1
        ldy     X1+1
        lda     Y1
        jsr     HPLOT
        bit     $C080           ; Switch in LC bank 2 for R/O
        rts

; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.
GETPIXEL:
        bit     $C082           ; Switch in ROM
        ldx     X1
        ldy     X1+1
        lda     Y1
        jsr     HPOSN
        lda     (HBASL),y
        and     HMASK
        asl
        beq     :+              ; 0 (black)
        lda     #$03            ; 3 (white)
:       bcc     :+
        adc     #$03            ; += 4 (black -> black2, white -> white2)
:       ldx     #$00
        bit     $C080           ; Switch in LC bank 2 for R/O
        rts

; LINE: Draw a line from X1/Y1 to X2/Y2, where X1/Y1 = ptr1/ptr2 and
; X2/Y2 = ptr3/ptr4 using the current drawing color.
; Must set an error code: NO
LINE:
        bit     $C082           ; Switch in ROM
        ldx     X1
        ldy     X1+1
        lda     Y1
        jsr     HPOSN
        lda     X2
        ldx     X2+1
        ldy     Y2
        jsr     HLIN
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
        inc     Y2
:       lda     Y2
        pha
        lda     Y1
        sta     Y2
        jsr     LINE
        pla
        sta     Y2
        inc     Y1
        cmp     Y1
        bne     :-
        rts

; TEXTSTYLE: Set the style used when calling OUTTEXT. Text scaling in X and Y
; direction is passend in X/Y, the text direction is passed in A.
; Must set an error code: NO
TEXTSTYLE:
        cmp     #TGI_TEXT_VERTICAL
        bne     :+
        lda     #48
:       sta     ROT
        rts

; OUTTEXT: Output text at X/Y = ptr1/ptr2 using the current color and the
; current text style. The text to output is given as a zero terminated
; string with address in ptr3.
; Must set an error code: NO
OUTTEXT:
        bit     $C082           ; Switch in ROM
        lda     X1
        ldy     X1+1
        ldx     ROT
        php                     ; Save Z flag
        beq     :+              ; Not horizontal
        sec
        sbc     #$07            ; Adjust X
        bcs     :+
        dey
:       tax
        lda     Y1
        plp                     ; Restore Z flag
        bne     :+              ; Not vertical
        sec
        sbc     #$07            ; Adjust Y
:       jsr     HPOSN
        clc
        lda     FONT+2*99       ; "connection char"
        adc     #<FONT
        sta     ptr4
        lda     FONT+2*99+1     ; "connection char"
        adc     #>FONT
        sta     ptr4+1
        ldy     #$00
:       lda     (ptr3),y
        beq     :+
        sty     tmp1            ; Save string index
        sec
        sbc     #$1F            ; No control chars
        asl                     ; Offset * 2
        tay
        clc
        lda     FONT,y
        adc     #<FONT
        tax
        lda     FONT+1,y
        adc     #>FONT
        tay
        lda     ROT
        jsr     DRAW            ; Draw char from string
        ldx     ptr4
        ldy     ptr4+1
        lda     ROT
        jsr     DRAW            ; Draw "connection char"
        ldy     tmp1            ; Restore string index
        iny
        bne     :-              ; Branch always
:       bit     $C080           ; Switch in LC bank 2 for R/O
        rts
