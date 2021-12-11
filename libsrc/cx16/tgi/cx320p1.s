;
; Graphics driver for the 320 pixels across, 200 pixels down, 256 colors mode
; on the Commander X16
;
; 2020-07-02, Greg King <gregdk@users.sf.net>
;

        .include        "zeropage.inc"

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .include        "cbm_kernal.inc"
        .include        "cx16.inc"

        .macpack        generic
        .macpack        module


; Macro that copies a word into a pseudo-register

.mac    setReg  reg, src
        lda     src
        ldx     src+1
        sta     gREG::reg
        stx     gREG::reg+1
.endmac


; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

        module_header   _cx320p1_tgi    ; 320 pixels across, 1 pixel per byte

; First part of the header is a structure that has a signature,
; and defines the capabilities of the driver.

        .byte   $74, $67, $69           ; ASCII "tgi"
        .byte   TGI_API_VERSION         ; TGI API version number
        .addr   $0000                   ; Library reference
        .word   320                     ; X resolution
        .word   200                     ; Y resolution
        .byte   <$0100                  ; Number of drawing colors
        .byte   1                       ; Number of screens available
        .byte   8                       ; System font X size
        .byte   8                       ; System font Y size
        .word   $0100                   ; Aspect ratio (based on VGA display)
        .byte   0                       ; TGI driver flags

; Next, comes the jump table. Currently, all entries must be valid,
; and may point to an RTS for test versions (function not implemented).

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


; ------------------------------------------------------------------------
; Constant

GRAPH320        =       $80

; ------------------------------------------------------------------------
; Data

; Variables mapped to the zero-page segment variables. Some of these are
; used for passing parameters to the driver.

X1              := ptr1
Y1              := ptr2
X2              := ptr3
Y2              := ptr4

; Absolute variables used in the code

.bss

; The colors are indicies into a TGI palette.  The TGI palette is indicies into
; VERA's palette.  Vera's palette is a table of Red, Green, and Blue levels.
; The first 16 RGB elements mimic the Commodore 64's colors.

defpalette:     .res    $0100
palette:        .res    $0100

bcolor          :=      palette + 0     ; Background color
color:          .res    1               ; Stroke and fill index
mode:           .res    1               ; Old text mode

.data

error:          .byte   TGI_ERR_OK      ; Error code


.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
;
; Must set an error code: NO

INSTALL:
; Create the default palette.

        ldx     #$00
:       txa
        sta     defpalette,x
        inx
        bnz     :-

        ; Fall through.

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory. May
; clean up anything done by INSTALL, but is probably empty most of the time.
;
; Must set an error code: NO

UNINSTALL:
        rts

; ------------------------------------------------------------------------
; INIT: Changes an already installed device from text mode to graphics
; mode.
; Note that INIT/DONE may be called multiple times while the driver
; is loaded, while INSTALL is called only once; so, any code that is needed
; to initiate variables and so on must go here.  Setting the palette is not
; needed because that is called by the graphics kernel later.
; The graphics kernel never will call INIT when a graphics mode already is
; active, so there is no need to protect against that.
;
; Must set an error code: YES

INIT:   stz     error           ; #TGI_ERR_OK

; Save the current text mode.

        lda     SCREEN_MODE
        sta     mode

; Switch into (320 x 200 x 256) graphics mode.

        lda     #GRAPH320
        jmp     SCREEN_SET_MODE

; ------------------------------------------------------------------------
; DONE: Will be called to switch the graphics device back into text mode.
; The graphics kernel never will call DONE when no graphics mode is active,
; so there is no need to protect against that.
;
; Must set an error code: NO

DONE:
; Work around a prerelease 37 Kernal bug.
; VERA (graphics) layer 0 isn't disabled by SCREEN_SET_MODE.

        stz     VERA::CTRL
        lda     VERA::DISP::VIDEO
        and     #<~VERA::DISP::ENABLE::LAYER0
        sta     VERA::DISP::VIDEO

        lda     mode
        jmp     SCREEN_SET_MODE

; ------------------------------------------------------------------------
; GETERROR: Return the error code in .A, and clear it.

GETERROR:
        lda     error
        stz     error
        rts

; ------------------------------------------------------------------------
; CONTROL: Platform-/driver-specific entry point.
;
; Must set an error code: YES

CONTROL:
        lda     #TGI_ERR_INV_FUNC
        sta     error
        rts

; ------------------------------------------------------------------------
; CLEAR: Clear the screen.
;
; Must set an error code: NO

CLEAR   :=      GRAPH_CLEAR

; ------------------------------------------------------------------------
; SETVIEWPAGE: Set the visible page. Called with the new page in .A (0..n-1).
; The page number already is checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will be called only if page OK)

SETVIEWPAGE:

        ; Fall through.

; ------------------------------------------------------------------------
; SETDRAWPAGE: Set the drawable page. Called with the new page in .A (0..n-1).
; The page number already is checked to be valid by the graphics kernel.
;
; Must set an error code: NO (will be called only if page OK)

SETDRAWPAGE:
        rts

; ------------------------------------------------------------------------
; SETPALETTE: Set the palette (not available with all drivers/hardware).
; A pointer to the palette is passed in ptr1. Must set an error if palettes
; are not supported
;
; Must set an error code: YES

SETPALETTE:
        stz     error           ; #TGI_ERR_OK
        ldy     #$00
:       lda     (ptr1),y
        sta     palette,y
        iny
        bnz     :-

        lda     color           ; Get stroke and fill index

        ; Fall through.

; ------------------------------------------------------------------------
; SETCOLOR: Set the drawing color (in .A). The new color already is checked
; to be in a valid range (0..maxcolor).
;
; Must set an error code: NO (will be called only if color OK)

SETCOLOR:
        tax
        sta     color
        lda     palette,x       ; Set stroke and fill color
        tax
        ldy     bcolor          ; Get background color
        jmp     GRAPH_SET_COLORS

; ------------------------------------------------------------------------
; GETPALETTE: Return the current palette in .XA. Even drivers that cannot
; set the palette should return the default palette here, so there's no
; way for this function to fail.
;
; Must set an error code: NO

GETPALETTE:
        lda     #<palette
        ldx     #>palette
        rts

; ------------------------------------------------------------------------
; GETDEFPALETTE: Return the default palette for the driver in .XA. All
; drivers should return something reasonable here, even drivers that don't
; support palettes, otherwise the caller has no way to determine the colors
; of the (not changable) palette.
;
; Must set an error code: NO (all drivers must have a default palette)

GETDEFPALETTE:
        lda     #<defpalette
        ldx     #>defpalette
        rts

; ------------------------------------------------------------------------
; SETPIXEL: Draw one pixel at X1/Y1 = ptr1/ptr2 with the current drawing
; color. The co-ordinates passed to this function never are outside the
; visible screen area, so there is no need for clipping inside this function.
;
; Must set an error code: NO

SETPIXEL:
        jsr     Point
        jsr     FB_CURSOR_POSITION
        ldx     color
        lda     palette,x
        jmp     FB_SET_PIXEL

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel, and return it in .XA. The
; co-ordinates passed to this function never are outside the visible screen
; area, so there is no need for clipping inside this function.

GETPIXEL:
        jsr     Point
        jsr     FB_CURSOR_POSITION
        jsr     FB_GET_PIXEL
        ldx     #>$0000
        rts

; ------------------------------------------------------------------------
; LINE: Draw a line from X1/Y1 to X2/Y2, where X1/Y1 = ptr1/ptr2 and
; X2/Y2 = ptr3/ptr4, using the current drawing color.
;
; Must set an error code: NO

LINE:   jsr     Point
        setReg  r2, X2
        setReg  r3, Y2
        jmp     GRAPH_DRAW_LINE

; ------------------------------------------------------------------------
; BAR: Draw a filled rectangle with the corners X1/Y1, X2/Y2, where
; X1/Y1 = ptr1/ptr2 and X2/Y2 = ptr3/ptr4, using the current drawing color.
; Contrary to most other functions, the graphics kernel will sort and clip
; the co-ordinates before calling the driver; so on entry, the following
; conditions are valid:
;       X1 <= X2
;       Y1 <= Y2
;       (X1 >= 0) && (X1 < XRES)
;       (X2 >= 0) && (X2 < XRES)
;       (Y1 >= 0) && (Y1 < YRES)
;       (Y2 >= 0) && (Y2 < YRES)
;
; Must set an error code: NO

BAR:
; Set the starting corner.

        jsr     Point

; Set the width.

        lda     X2
        sub     X1
        sta     gREG::r2
        lda     X2+1
        sbc     X1+1
        sta     gREG::r2+1

; Set the height.

        lda     Y2
        sub     Y1
        sta     gREG::r3
        lda     Y2+1
        sbc     Y1+1
        sta     gREG::r3+1

; Set the corner radius.

        stz     gREG::r4
        stz     gREG::r4+1

        sec                     ; Fill the rectangle
        jmp     GRAPH_DRAW_RECT

; ------------------------------------------------------------------------
; TEXTSTYLE: Set the style used when calling OUTTEXT. Text scaling in X and Y
; directions are passed in .X and .Y, the text direction is passed in .A.
;
; Must set an error code: NO

TEXTSTYLE:
        rts

; ------------------------------------------------------------------------
; OUTTEXT: Output text at X/Y = ptr1/ptr2 using the current color and the
; current text style. The text to output is given as a zero-terminated
; string with address in ptr3.
;
; Must set an error code: NO

OUTTEXT:
        jsr     Point

        ldy     #$00
@next:  lda     (ptr3),y
        bze     @end
        phy
        jsr     GRAPH_PUT_CHAR
        ply
        iny
        bnz     @next
@end:   rts

; ------------------------------------------------------------------------
; Point: Set the arguments for the first point of a Kernal graphics function.

Point:  setReg  r0, X1
        setReg  r1, Y1
        rts
