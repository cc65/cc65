;
; Graphics driver for a 160x192x2 mode on the VIC-20.
;
; Based on C64 TGI driver
;
; 2018-03-11, Sven Michael Klose
; 2020-07-06, Greg King
;

        .include        "zeropage.inc"
        .include        "vic20.inc"

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Constants

COLS    =       20
ROWS    =       12
XRES    =       COLS * 8
YRES    =       ROWS * 16

TGI_IOCTL_VIC20_SET_PATTERN     =       $01

; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

        module_header   _vic20_hi_tgi

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

        .byte   $74, $67, $69           ; ASCII "tgi"
        .byte   TGI_API_VERSION         ; TGI API version number
        .addr   $0000                   ; Library reference
        .word   XRES                    ; X resolution
        .word   YRES                    ; Y resolution
        .byte   2                       ; Number of drawing colors
        .byte   1                       ; Number of screens available
        .byte   8                       ; System font X size
        .byte   8                       ; System font Y size
        .word   $0180                   ; Aspect ratio 2.5:3
        .byte   0                       ; TGI driver flags

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

; ------------------------------------------------------------------------
; Data.

; Variables mapped to the zero-page segment variables. Some of these are
; used for passing parameters to the driver.

X1              := ptr1
Y1              := ptr2
X2              := ptr3
Y2              := ptr4
TEXT            := ptr3

POINT           := regsave
SOURCE          := tmp1
DEST            := tmp3

; Absolute variables used in the code

.bss

ERROR:          .res    1       ; Error code
PALETTE:        .res    2       ; The current palette

CURCOL:         .res    1       ; Current color.
BITMASK:        .res    1       ; $00 = clear, $FF = set pixels

; BAR variables

XPOSR:          .res    1       ; Used by BAR.
PATTERN:        .res    2       ; Address of pattern.
USERPATTERN:    .res    2       ; User-defined pattern set via CONTROL.
COUNTER:        .res    2
TMP:            .res    1
MASKS:          .res    1
MASKD:          .res    1
XCPOS:          .res    1
HEIGHT:         .res    1

; Line variables

CHUNK           := X2           ; Used in the line routine
OLDCHUNK        := X2+1         ; Ditto
TEMP            := tmp4
TEMP2           := sreg
DX:             .res    2
DY:             .res    2

; Text output stuff

TEXTMAGX:       .res    1
TEXTMAGY:       .res    1
TEXTDIR:        .res    1

; Constants and tables

.rodata

DEFPALETTE:     .byte   $00, $01        ; White on black
PALETTESIZE     =       * - DEFPALETTE

BITTAB:         .byte   $80, $40, $20, $10, $08, $04, $02, $01
BITCHUNK:       .byte   $FF, $7F, $3F, $1F, $0F, $07, $03, $01

CHARROM         := $8000        ; Character ROM base address
CBASE           := $9400        ; Color memory base address
SBASE           := $1000        ; Screen memory base address
.assert (<SBASE) = $0, error, "Error, SBASE must be page aligned"
VBASE           := $1100        ; Video memory base address

; These numbers are added to Kernal's default VIC settings.
; They make the Video Interface Controller show graphics instead of text.

.proc   VICREGS
        .byte   <+2             ; Left_edge + 2
        .byte   <-2             ;  Top_edge - 2
        .byte   <-2             ; Columns - 2
        .byte   <-(11 << 1) + 1 ; Rows - 11, chars. are 8 x 16 pixels
        .byte   0
        .byte   <+$0C           ; Font_address = $1000
.endproc

XADDRS_L:
        .repeat COLS, n
        .byte   <(VBASE + YRES * n)
        .endrep

XADDRS_H:
        .repeat COLS, n
        .byte   >(VBASE + YRES * n)
        .endrep

MASKS_LEFT:
        .byte %11111111
MASKD_RIGHT:
        .byte %01111111
        .byte %00111111
        .byte %00011111
        .byte %00001111
        .byte %00000111
        .byte %00000011
        .byte %00000001
MASKD_LEFT:
        .byte %00000000
MASKS_RIGHT:
        .byte %10000000
        .byte %11000000
        .byte %11100000
        .byte %11110000
        .byte %11111000
        .byte %11111100
        .byte %11111110
        .byte %11111111

PATTERN_EMPTY:
        .byte %00000000
        .byte %00000000
        .byte %00000000
        .byte %00000000
        .byte %00000000
        .byte %00000000
        .byte %00000000
        .byte %00000000

PATTERN_SOLID:
        .byte %11111111
        .byte %11111111
        .byte %11111111
        .byte %11111111
        .byte %11111111
        .byte %11111111
        .byte %11111111
        .byte %11111111

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
;
; Must set an error code: NO
;

.proc INSTALL

; Reset user-defined pattern.

        lda     #$00
        sta     PATTERN
        sta     PATTERN+1
        rts
.endproc

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory. May
; clean up anything done by INSTALL; but is probably empty most of the time.
;
; Must set an error code: NO
;

.proc UNINSTALL
        rts
.endproc

; ------------------------------------------------------------------------
; INIT: Changes an already installed device from text mode to graphics
; mode.
; Note that INIT/DONE may be called multiple times while the driver
; is loaded, while INSTALL is called only once, so any code that is needed
; to initialize variables and so on must go here. Setting the palette
; is not needed because that is done by the graphics kernel later.
; The graphics kernel never will call INIT when a graphics mode already is
; active; so, there is no need to protect against that.
;
; Must set an error code: YES
;

.proc INIT

; Initialize variables

        ldy     #$FF
        sty     BITMASK
        iny                     ; (ldy #$00)
        sty     ERROR           ; Set to TGI_ERR_OK

; Make screen columns.

        sty     tmp2
        lda     #>SBASE
        sta     tmp2+1
        clc
        ldx     #$10

@NEXT_ROW:
        txa

@NEXT_COLUMN:
        sta     (tmp2),y
        iny
        adc     #ROWS
        bcc     @NEXT_COLUMN

; Step to next row on screen.

        inx
        cpx     #ROWS+$10
        bne     @NEXT_ROW

; Set up VIC.

        ldx     #.sizeof(VICREGS) - 1
@L2:    clc
        lda     $EDE4,x
        adc     VICREGS,x
        sta     VIC,x
        dex
        bpl     @L2

        jmp     CLEAR
.endproc

; ------------------------------------------------------------------------
; DONE: Will be called to switch the graphics device back into text mode.
; The graphics kernel never will call DONE when no graphics mode is active;
; so, there is no need to protect against that.
;
; Must set an error code: NO
;

.proc DONE
        jmp     $E518           ; Kernal console init.
.endproc

; ------------------------------------------------------------------------
; GETERROR: Return the error code in A, and clear it.

.proc GETERROR
        ldx     #TGI_ERR_OK
        lda     ERROR
        stx     ERROR
        rts
.endproc

; ------------------------------------------------------------------------
; CONTROL: Platform-/driver-specific entry point.
;
; Must set an error code: YES
;

.proc CONTROL

; Set user-defined pattern.

        cmp     #TGI_IOCTL_VIC20_SET_PATTERN
        bne     @INVALID_FUNC

        lda     ptr1
        sta     USERPATTERN
        lda     ptr1+1
        sta     USERPATTERN+1

        lda     #TGI_ERR_OK
        .byte   $2C             ;(bit $1234)

; Return with error code for invalid function index.

@INVALID_FUNC:
        lda     #TGI_ERR_INV_FUNC
        sta     ERROR
        rts
.endproc

; ------------------------------------------------------------------------
; CLEAR: Clears the screen.  All pixels are set to the background color.
;
; Must set an error code: NO
;

.proc CLEAR
        lda     #%00000000
        tay                     ; (ldy #$00)
@L1:    sta     VBASE + $0000,y
        sta     VBASE + $0100,y
        sta     VBASE + $0200,y
        sta     VBASE + $0300,y
        sta     VBASE + $0400,y
        sta     VBASE + $0500,y
        sta     VBASE + $0600,y
        sta     VBASE + $0700,y
        sta     VBASE + $0800,y
        sta     VBASE + $0900,y
        sta     VBASE + $0A00,y
        sta     VBASE + $0B00,y
        sta     VBASE + $0C00,y
        sta     VBASE + $0D00,y
        sta     VBASE + $0E00,y
        iny
        bne     @L1
        rts
.endproc

; ------------------------------------------------------------------------
; SETVIEWPAGE: Set the visible page. Called with the new page in .A (0..n).
; The page number already is checked, to be valid, by the graphics kernel.
;
; Must set an error code: NO (will be called only if page OK)
;

.proc SETVIEWPAGE
        rts
.endproc

; ------------------------------------------------------------------------
; SETDRAWPAGE: Set the drawable page. Called with the new page in .A (0..n-1).
; The page number already is checked, to be valid, by the graphics kernel.
;
; Must set an error code: NO (will be called only if page OK)
;

.proc SETDRAWPAGE
        rts
.endproc

; ------------------------------------------------------------------------
; SETCOLOR: Set the drawing color (in .A). The new color already is checked
; to be in a valid range (0..maxcolor).
;
; Must set an error code: NO (will be called only if color OK)
;

.proc SETCOLOR
        sta     CURCOL
        tax
        beq     @L1
        lda     #$FF
@L1:    sta     BITMASK
        rts
.endproc

; ------------------------------------------------------------------------
; SETPALETTE: Set the palette (not available with all drivers/hardware).
; A pointer to the palette is passed in ptr1. Must set an error if palettes
; are not supported
;
; Must set an error code: YES
;

.proc SETPALETTE
        ldy     #PALETTESIZE - 1
@L1:    lda     (ptr1),y        ; Copy the palette
        and     #$0F            ; Make a valid color
        sta     PALETTE,y
        dey
        bpl     @L1

; Initialize the color map with the new color settings.

        iny                     ; Set .Y to $00
        lda     PALETTE+1       ; Foreground color
@L2:    sta     CBASE + $0000,y
        sta     CBASE + $0100,y
        iny
        bne     @L2

        lda     PALETTE         ; Background color
        asl     a               ; Put it in high nybble
        asl     a
        asl     a
        asl     a
        sta     tmp2
        lda     VIC_COLOR
        and     #$0F
        ora     tmp2
        sta     VIC_COLOR

; Done, reset the error code

        ;ldy    #TGI_ERR_OK     ; (Already zero)
        sty     ERROR
        rts
.endproc

; ------------------------------------------------------------------------
; GETPALETTE: Return the current palette in ".XA". Even drivers that cannot
; set the palette should return the default palette here; so, there's no
; way for this function to fail.
;
; Must set an error code: NO
;

.proc GETPALETTE
        lda     #<PALETTE
        ldx     #>PALETTE
        rts
.endproc

; ------------------------------------------------------------------------
; GETDEFPALETTE: Return the default palette for the driver in ".XA". All
; drivers should return something reasonable here, even drivers that don't
; support palettes; otherwise, the caller has no way to determine the colors
; of the (not changeable) palette.
;
; Must set an error code: NO (all drivers must have a default palette)
;

.proc GETDEFPALETTE
        lda     #<DEFPALETTE
        ldx     #>DEFPALETTE
        rts
.endproc

; ------------------------------------------------------------------------
; SETPIXEL: Draw one pixel at X1/Y1 = ptr1/ptr2 with the current drawing
; color. The co-ordinates passed to this function never are outside the
; visible screen area; so, there's no need for clipping inside this function.
;
; Must set an error code: NO
;

.proc SETPIXEL
        jsr     CALC            ; Calculate co-ordinates

        ldy     #$00
        lda     (POINT),Y
        eor     BITMASK
        and     BITTAB,X
        eor     (POINT),Y
        sta     (POINT),Y
        rts
.endproc

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel; and return it in ".XA". The
; co-ordinates passed to this function never are outside the visible screen
; area; so, there's no need for clipping inside this function.


.proc GETPIXEL
        jsr     CALC            ; Calculate co-ordinates

        ldy     #$00
        lda     (POINT),Y
        and     BITTAB,X
        beq     @L1
        iny
        tya                     ; Get color value into .A
@L1:    ldx     #>$0000         ; Clear high byte
        rts
.endproc

; ------------------------------------------------------------------------
; TEXTSTYLE: Set the style used when calling OUTTEXT. Text scalings in x and y
; directions are passed in .X/.Y; the text direction is passed in .A.
;
; Must set an error code: NO
;

.proc TEXTSTYLE
        stx     TEXTMAGX
        sty     TEXTMAGY
        sta     TEXTDIR
        rts
.endproc

; ------------------------------------------------------------------------
; OUTTEXT: Output text at X/Y = ptr1/ptr2 using the current color and the
; current text style. The text to output is given as a zero-terminated
; string with address in ptr3.
;
; Must set an error code: NO
;

.proc OUTTEXT
        rts
.endproc

; ------------------------------------------------------------------------
; Calculate address and X offset in char. line, to plot the pixel at X1/Y1.

.proc CALC
        lda     X1+1
        bne     @L1
        lda     Y1+1
        bne     @L1

        lda     X1
        lsr
        lsr
        lsr
        tay

        lda     XADDRS_L,y
        clc
        adc     Y1
        sta     POINT
        lda     XADDRS_H,y
        adc     #$00
        sta     POINT+1

        lda     X1
        and     #7
        tax

@L1:    rts
.endproc

; ------------------------------------------------------------------------
; LINE: Draw a line from X1/Y1 to X2/Y2, where X1/Y1 = ptr1/ptr2 and
; X2/Y2 = ptr3/ptr4 using the current drawing color.
;
; X1,X2 etc. are set up above (x2=LINNUM in particular)
; Format is LINE x2,y2,x1,y1
;
; Must set an error code: NO
;

.proc LINE

@CHECK: lda     X2              ; Make sure x1 < x2
        sec
        sbc     X1
        tax
        lda     X2+1
        sbc     X1+1
        bpl     @CONT
        lda     Y2              ; If not, swap P1 and P2
        ldy     Y1
        sta     Y1
        sty     Y2
        lda     Y2+1
        ldy     Y1+1
        sta     Y1+1
        sty     Y2+1
        lda     X1
        ldy     X2
        sty     X1
        sta     X2
        lda     X2+1
        ldy     X1+1
        sta     X1+1
        sty     X2+1
        bcc     @CHECK

@CONT:  sta     DX+1
        stx     DX

        ldx     #$C8            ; INY
        lda     Y2              ; Calculate dy
        sec
        sbc     Y1
        tay
        lda     Y2+1
        sbc     Y1+1
        bpl     @DYPOS          ; Is y2 >= y1?
        lda     Y1              ; Otherwise, dy = y1 - y2
        sec
        sbc     Y2
        tay
        ldx     #$88            ; DEY

@DYPOS: sty     DY              ; 8-bit DY -- FIX ME?
        stx     YINCDEC
        stx     XINCDEC

        lda     X1
        lsr
        lsr
        lsr
        tay
        lda     XADDRS_L,y
        sta     POINT
        lda     XADDRS_H,y
        sta     POINT+1
        ldy     Y1

        lda     X1
        and     #7
        tax
        lda     BITCHUNK,X
        sta     OLDCHUNK
        sta     CHUNK

        ldx     DY
        cpx     DX              ; Who's bigger: dy or dx?
        bcc     STEPINX         ; If dx, then...
        lda     DX+1
        bne     STEPINX

;
; Big steps in Y
;
;   X is now counter, Y is y co-ordinate
;
; On entry, X=DY=number of loop iterations, and Y=Y1
STEPINY:
        lda     #$00
        sta     OLDCHUNK        ; So plotting routine will work right
        lda     CHUNK
        lsr                     ; Strip the bit
        eor     CHUNK
        sta     CHUNK
        txa
        bne     @CONT           ; If dy=0, it's just a point
        inx
@CONT:  lsr                     ; Init. counter to dy/2
;
; Main loop
;
YLOOP:  sta     TEMP

        lda     (POINT),y       ; Plot
        eor     BITMASK
        and     CHUNK
        eor     (POINT),y
        sta     (POINT),y
YINCDEC:
        iny                     ; Advance Y co-ordinate
        lda     TEMP            ; Restore A
        sec
        sbc     DX
        bcc     YFIXX
YCONT:  dex                     ; X is counter
        bne     YLOOP
YCONT2: lda     (POINT),y       ; Plot endpoint
        eor     BITMASK
        and     CHUNK
        eor     (POINT),y
        sta     (POINT),y
        rts

YFIXX:                          ; x=x+1
        adc     DY
        lsr     CHUNK
        bne     YCONT           ; If we pass a column boundary,
        ror     CHUNK           ; then reset CHUNK to $80
        sta     TEMP2
        lda     POINT
        adc     #YRES
        sta     POINT
        bcc     @CONT
        inc     POINT+1
@CONT:  lda     TEMP2
        dex
        bne     YLOOP
        beq     YCONT2

;
; Big steps in X direction
;
; On entry, X=DY=number of loop iterations, and Y=Y1

.bss
COUNTHI:
        .byte   $00             ; Temporary counter,
                                ; used only once
.code
STEPINX:
        ldx     DX
        lda     DX+1
        sta     COUNTHI
        cmp     #$80
        ror                     ; Need bit for initialization
        sta     Y1              ; High byte of counter
        txa
        bne     @CONT           ; Could be $100
        dec     COUNTHI
@CONT:  ror
;
; Main loop
;
XLOOP:  lsr     CHUNK
        beq     XFIXC           ; If we pass a column boundary...
XCONT1: sbc     DY
        bcc     XFIXY           ; Time to step in Y?
XCONT2: dex
        bne     XLOOP
        dec     COUNTHI         ; High bits set?
        bpl     XLOOP

        lsr     CHUNK           ; Advance to last point
        jmp     LINEPLOT        ; Plot the last chunk
;
; CHUNK has passed a column; so, plot and increment pointer;
; and fix up CHUNK, OLDCHUNK.
;
XFIXC:  sta     TEMP
        jsr     LINEPLOT
        lda     #$FF
        sta     CHUNK
        sta     OLDCHUNK
        lda     POINT
        clc
        adc     #YRES
        sta     POINT
        lda     TEMP
        bcc     XCONT1
        inc     POINT+1
        jmp     XCONT1
;
; Check to make sure there isn't a high bit; plot chunk;
; and update Y co-ordinate.
;
XFIXY:  dec     Y1              ; Maybe high bit set
        bpl     XCONT2
        adc     DX
        sta     TEMP
        lda     DX+1
        adc     #$FF            ; Hi byte
        sta     Y1

        jsr     LINEPLOT        ; Plot chunk
        lda     CHUNK
        sta     OLDCHUNK

        lda     TEMP
XINCDEC:
        iny                     ; Y co-ord
        jmp     XCONT2

;
; Subroutine to plot chunks/points (to save a little
; room, gray hair, etc.)
;
LINEPLOT:                       ; Plot the line chunk
        lda     (POINT),Y
        eor     BITMASK
        ora     CHUNK
        and     OLDCHUNK
        eor     CHUNK
        eor     (POINT),Y
        sta     (POINT),Y
        rts
.endproc

; In: xpos, ypos, width, height
; ------------------------------------------------------------------------
; BAR: Draw a filled rectangle with the corners X1/Y1, X2/Y2, where
; X1/Y1 = ptr1/ptr2 and X2/Y2 = ptr3/ptr4 using the current drawing color.
; Contrary to most other functions, the graphics kernel will sort and clip
; the co-ordinates before calling the driver; so, on entry, the following
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

.proc BAR

; Set user pattern if available.

        lda     USERPATTERN
        ora     USERPATTERN+1
        beq     @GET_PATTERN_BY_COLOR

        lda     USERPATTERN
        sta     PATTERN
        lda     USERPATTERN+1
        sta     PATTERN+1
        jmp     @GOT_PATTERN

; Determine pattern based on current colour.

@GET_PATTERN_BY_COLOR:
        lda     #<PATTERN_SOLID
        ldx     #>PATTERN_SOLID
        ldy     CURCOL
        bne     @L2
        lda     #<PATTERN_EMPTY
        ldx     #>PATTERN_EMPTY
@L2:    sta     PATTERN
        stx     PATTERN+1

@GOT_PATTERN:

; Get starting POINT on screen.

        jsr     CALC
        sty     XCPOS
        lda     POINT           ; One off for VFILL/VCOPY.
        sec
        sbc     #1
        sta     POINT
        bcs     @L3
        dec     POINT+1
@L3:

; Get height for VFILL.

        lda     Y2
        sec
        sbc     Y1
        sta     HEIGHT

; Get rightmost char column.

        lda     X2
        and     #7
        sta     XPOSR

; Get width in characters.

        lda     X2
        lsr
        lsr
        lsr
        sec
        sbc     XCPOS
        beq     @DRAW_SINGLE_COLUMN
        sta     COUNTER

; Draw left end.

        lda     X1
        and     #7
        tax
        lda     MASKD_LEFT,x
        sta     MASKD
        lda     MASKS_LEFT,x
        sta     MASKS
        jsr     VFILL
        jsr     INCPOINTX

; Draw middle.

        dec     COUNTER
        beq     @DRAW_RIGHT_END
@L1:    jsr     VCOPY
        jsr     INCPOINTX
        dec     COUNTER
        bne     @L1

; Draw right end.

@DRAW_RIGHT_END:
        ldx     XPOSR
        lda     MASKD_RIGHT,x
        sta     MASKD
        lda     MASKS_RIGHT,x
        sta     MASKS
        jmp     VFILL

; Draw left end.

@DRAW_SINGLE_COLUMN:
        lda     X1
        and     #7
        tax
        ldy     XPOSR
        lda     MASKS_LEFT,x
        and     MASKS_RIGHT,y
        sta     MASKS
        lda     MASKD_LEFT,x
        ora     MASKD_RIGHT,y
        sta     MASKD
        jmp     VFILL
.endproc

; In:   HEIGHT, PATTERN
;       MASKS:  Source mask (ANDed with pattern).
;       MASKD:  Destination mask (ANDed with screen).
;       POINT:  Starting address.
; ------------------------------------------------------------------------
; Fill column with pattern using masks.
;

.proc VFILL
        lda     PATTERN
        sta     @MOD_PATTERN+1
        lda     PATTERN+1
        sta     @MOD_PATTERN+2
        ldy     HEIGHT
        lda     Y1
        and     #7
        tax

@L1:    lda     (POINT),y
        and     MASKD
        sta     TMP
@MOD_PATTERN:
        lda     $FFFF,x
        and     MASKS
        ora     TMP
        sta     (POINT),y
        inx
        txa
        and     #7
        tax
        dey
        bne     @L1

        rts
.endproc

; In:   HEIGHT, PATTERN, POINT
; ------------------------------------------------------------------------
; Fill column with pattern.
;

.proc VCOPY
        lda     PATTERN
        sta     @MOD_PATTERN+1
        lda     PATTERN+1
        sta     @MOD_PATTERN+2
        ldy     HEIGHT
        lda     Y1
        and     #7
        tax

@MOD_PATTERN:
@L1:    lda     $FFFF,x
        sta     (POINT),y
        inx
        txa
        and     #7
        tax
        dey
        bne     @L1

        rts
.endproc

.proc INCPOINTX
        lda     POINT
        clc
        adc     #YRES
        sta     POINT
        bcc     @L1
        inc     POINT+1
@L1:    rts
.endproc
