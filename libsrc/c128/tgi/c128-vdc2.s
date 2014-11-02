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

        .include        "zeropage.inc"

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Constants

VDC_ADDR_REG      = $D600                 ; VDC address
VDC_DATA_REG      = $D601                 ; VDC data

VDC_DSP_HI        = 12                    ; registers used
VDC_DSP_LO        = 13
VDC_DATA_HI       = 18
VDC_DATA_LO       = 19
VDC_VSCROLL       = 24
VDC_HSCROLL       = 25
VDC_COLORS        = 26
VDC_CSET          = 28
VDC_COUNT         = 30
VDC_DATA          = 31

; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

        module_header   _c128_vdc2_tgi

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

        .byte   $74, $67, $69           ; "tgi"
        .byte   TGI_API_VERSION         ; TGI API version number
        .addr   $0000                   ; Library reference
xres:   .word   640                     ; X resolution
yres:   .word   480                     ; Y resolution
        .byte   2                       ; Number of drawing colors
pages:  .byte   0                       ; Number of screens available
        .byte   8                       ; System font X size
        .byte   8                       ; System font Y size
        .word   $0100                   ; Aspect ratio (based on 4/3 display)
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

X1              = ptr1
Y1              = ptr2
X2              = ptr3
Y2              = ptr4

ADDR            = tmp1
TEMP            = tmp3
TEMP2           = tmp4          ; HORLINE
TEMP3           = sreg          ; HORLINE

; Absolute variables used in the code

.bss

ERROR:          .res    1       ; Error code
PALETTE:        .res    2       ; The current palette

BITMASK:        .res    1       ; $00 = clear, $FF = set pixels

OLDCOLOR:       .res    1       ; colors before entering gfx mode

; Text output stuff
TEXTMAGX:       .res    1
TEXTMAGY:       .res    1
TEXTDIR:        .res    1

; Constants and tables

.rodata

DEFPALETTE:     .byte   $00, $0f        ; White on black
PALETTESIZE     = * - DEFPALETTE

BITTAB:         .byte   $80,$40,$20,$10,$08,$04,$02,$01

BITMASKL:       .byte   %11111111, %01111111, %00111111, %00011111
                .byte   %00001111, %00000111, %00000011, %00000001

BITMASKR:       .byte   %10000000, %11000000, %11100000, %11110000
                .byte   %11111000, %11111100, %11111110, %11111111

; color translation table (indexed by VIC color)
COLTRANS:       .byte $00, $0f, $08, $06, $0a, $04, $02, $0c
                .byte $0d, $0b, $09, $01, $0e, $05, $03, $07
                ; colors BROWN and GRAY3 are wrong

; VDC initialization table (reg),(val),...,$ff
InitVDCTab:
                .byte VDC_DSP_HI, 0             ; viewpage 0 as default
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

SCN80CLR:       .byte 27,88,147,27,88,0

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

        ldx     #VDC_CSET       ; determine size of RAM...
        jsr     VDCReadReg
        sta     tmp1
        ora     #%00010000
        jsr     VDCWriteReg     ; turn on 64k

        jsr     settestadr1     ; save original value of test byte
        jsr     VDCReadByte
        sta     tmp2

        lda     #$55            ; write $55 here
        ldy     #ptr1
        jsr     test64k         ; read it here and there
        lda     #$aa            ; write $aa here
        ldy     #ptr2
        jsr     test64k         ; read it here and there

        jsr     settestadr1
        lda     tmp2
        jsr     VDCWriteByte    ; restore original value of test byte

        lda     ptr1            ; do bytes match?
        cmp     ptr1+1
        bne     @have64k
        lda     ptr2
        cmp     ptr2+1
        bne     @have64k

        ldx     #VDC_CSET
        lda     tmp1
        jsr     VDCWriteReg     ; restore 16/64k flag
        jmp     @endok          ; and leave default values for 16k

@have64k:
        lda     #1
        sta     pages
@endok:
        rts

test64k:
        sta     tmp1
        sty     ptr3
        lda     #0
        sta     ptr3+1
        jsr     settestadr1
        lda     tmp1
        jsr     VDCWriteByte            ; write $55
        jsr     settestadr1
        jsr     VDCReadByte             ; read here
        pha
        jsr     settestadr2
        jsr     VDCReadByte             ; and there
        ldy     #1
        sta     (ptr3),y
        pla
        dey
        sta     (ptr3),y
        rts

settestadr1:
        ldy     #$02                    ; test page 2 (here)
        .byte   $2c
settestadr2:
        ldy     #$42                    ; or page 64+2 (there)
        lda     #0
        jmp     VDCSetSourceAddr

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
        lda     pages                   ; is there enough memory?
        bne     @L1                     ; Jump if there is one screen
        lda     #TGI_ERR_INV_MODE       ; Error
        bne     @L9

; Initialize variables

@L1:    ldx     #$FF
        stx     BITMASK

; Remeber current color value
        ldx     #VDC_COLORS
        jsr     VDCReadReg
        sta     OLDCOLOR

; Switch into graphics mode (set view page 0)

        ldy     #0
@L2:    ldx     InitVDCTab,y
        bmi     @L3
        iny
        lda     InitVDCTab,y
        jsr     VDCWriteReg
        iny
        bne     @L2
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
        jsr $e179               ; reload character set and setup VDC
        jsr $ff62
        lda $d7                 ; in 80-columns?
        bne @L01
@L0:    lda SCN80CLR,y
        beq @L1
        jsr $ffd2               ; print \xe,clr,\xe
        iny
        bne @L0
@L01:   lda #147
        jsr $ffd2               ; print clr
@L1:    lda #0                  ; restore view page
        ldx #VDC_DSP_HI
        jsr VDCWriteReg
        lda OLDCOLOR
        ldx #VDC_COLORS
        jsr VDCWriteReg         ; restore color (background)
        lda #$47
        ldx #VDC_HSCROLL
        jmp VDCWriteReg         ; switch to text screen

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
        jsr     VDCSetSourceAddr
        lda     #0
        ldx     #VDC_VSCROLL
        jsr     VDCWriteReg                     ; set fill mode
        lda     #0
        jsr     VDCWriteByte                    ; put 1rst byte (fill value)
        ldy     #159                            ; 159 times
        lda     #0                              ; 256 bytes
        ldx     #VDC_COUNT
@L1:    jsr     VDCWriteReg
        dey
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

; Get the color entries from the palette

        ldy     PALETTE+1       ; Foreground color
        lda     COLTRANS,y
        asl     a
        asl     a
        asl     a
        asl     a
        ldy     PALETTE         ; Background color
        ora     COLTRANS,y

        ldx     #VDC_COLORS
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

SETPIXEL:
        jsr     CALC            ; Calculate coordinates

        stx     TEMP
        lda     ADDR
        ldy     ADDR+1
        jsr     VDCSetSourceAddr
        jsr     VDCReadByte
        ldx     TEMP

        sta     TEMP
        eor     BITMASK
        and     BITTAB,X
        eor     TEMP
        pha
        lda     ADDR
        ldy     ADDR+1
        jsr     VDCSetSourceAddr
        pla
        jsr     VDCWriteByte

@L9:    rts

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.


GETPIXEL:
        jsr     CALC            ; Calculate coordinates

        stx     TEMP            ; preserve X
        lda     ADDR
        ldy     ADDR+1
        jsr     VDCSetSourceAddr
        jsr     VDCReadByte
        ldx     TEMP

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

BAR:
        inc     Y2
        bne     HORLINE
        inc     Y2+1

; Original code for a horizontal line

HORLINE:
        lda X1
        pha
        lda X1+1
        pha
        jsr CALC                ; get data for LEFT
        lda BITMASKL,x          ; remember left address and bitmask
        pha
        lda ADDR
        pha
        lda ADDR+1
        pha

        lda X2
        sta X1
        lda X2+1
        sta X1+1
        jsr CALC                ; get data for RIGHT
        lda BITMASKR,x
        sta TEMP3

        pla                     ; recall data for LEFT
        sta X1+1
        pla
        sta X1                  ; put left address into X1
        pla

        cmp #%11111111          ; if left bit <> 0
        beq @L1
        sta TEMP2               ; do left byte only...
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
        inc X1                  ; ... and proceed
        bne @L1
        inc X1+1

        ; do right byte (if Y2=0 ++ADDR and skip)
@L1:    lda TEMP3
        cmp #%11111111          ; if right bit <> 7
        bne @L11
        inc ADDR                ; right bit = 7 - the next one is the last
        bne @L10
        inc ADDR+1
@L10:   bne @L2

@L11:   lda ADDR                ; do right byte only...
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

@L2:                            ; do the fill in the middle
        lda ADDR                ; calculate offset in full bytes
        sec
        sbc X1
        beq @L3                 ; if equal - there are no more bytes
        sta ADDR

        lda X1                  ; setup for the left side
        ldy X1+1
        jsr VDCSetSourceAddr
        lda BITMASK             ; get color
        jsr VDCWriteByte        ; put 1st value
        ldx ADDR
        dex
        beq @L3                 ; 1 byte already written

        stx ADDR                ; if there are more bytes - fill them...
        ldx #VDC_VSCROLL
        lda #0
        jsr VDCWriteReg         ; setup for fill
        ldx #VDC_COUNT
        lda ADDR
        jsr VDCWriteReg         ; ... fill them NOW!

@L3:    pla
        sta X1+1
        pla
        sta X1

; End of horizontal line code

        inc     Y1
        bne     @L4
        inc     Y1+1
@L4:    lda     Y1
        cmp     Y2
        bne     @L5
        lda     Y1+1
        cmp     Y2+1
        bne     @L5
        rts

@L5:    jmp     HORLINE


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
        lda     Y1
        pha
        lda     Y1+1
        pha
        lsr
        ror     Y1              ; Y=Y/2
        sta     Y1+1
        sta     ADDR+1
        lda     Y1
        asl
        rol     ADDR+1
        asl
        rol     ADDR+1          ; Y*4
        clc
        adc     Y1
        sta     ADDR
        lda     Y1+1
        adc     ADDR+1
        sta     ADDR+1          ; Y*4+Y=Y*5
        lda     ADDR
        asl
        rol     ADDR+1
        asl
        rol     ADDR+1
        asl
        rol     ADDR+1
        asl
        rol     ADDR+1
        sta     ADDR            ; Y*5*16=Y*80
        lda     X1+1
        sta     TEMP
        lda     X1
        lsr     TEMP
        ror
        lsr     TEMP
        ror
        lsr     TEMP
        ror
        clc
        adc     ADDR
        sta     ADDR
        lda     ADDR+1          ; ADDR = Y*80+x/8
        adc     TEMP
        sta     ADDR+1
        pla
        sta     Y1+1
        pla
        sta     Y1
        and     #1
        beq     @even           ; even line - no offset
        lda     ADDR
        clc
        adc     #<21360
        sta     ADDR
        lda     ADDR+1
        adc     #>21360
        sta     ADDR+1          ; odd lines are 21360 bytes farther
@even:  lda     X1
        and     #7
        tax
        rts

;-------------
; VDC helpers

VDCSetSourceAddr:
        pha
        tya
        ldx     #VDC_DATA_HI
        jsr     VDCWriteReg
        pla
        ldx     #VDC_DATA_LO
        bne     VDCWriteReg

VDCReadByte:
        ldx     #VDC_DATA
VDCReadReg:
        stx     VDC_ADDR_REG
@L0:    bit     VDC_ADDR_REG
        bpl     @L0
        lda     VDC_DATA_REG
        rts

VDCWriteByte:
        ldx     #VDC_DATA
VDCWriteReg:
        stx     VDC_ADDR_REG
@L0:    bit     VDC_ADDR_REG
        bpl     @L0
        sta     VDC_DATA_REG
        rts

; ------------------------------------------------------------------------

        .include        "../../tgi/tgidrv_line.inc"
