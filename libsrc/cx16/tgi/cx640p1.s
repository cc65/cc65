;
; Graphics driver for the 640 pixels across, 480 pixels down, 2 color mode
; on the Commander X16
;
; 2024-06-11, Scott Hutter
; Based on code by Greg King
;

        .include        "zeropage.inc"

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .include        "cbm_kernal.inc"
        .include        "cx16.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

        module_header   _cx640p1_tgi    ; 640 pixels across, 1 pixel per bit

; First part of the header is a structure that has a signature,
; and defines the capabilities of the driver.

        .byte   $74, $67, $69           ; ASCII "tgi"
        .byte   TGI_API_VERSION         ; TGI API version number
        .addr   $0000                   ; Library reference
        .word   640                     ; X resolution
        .word   480                     ; Y resolution
        .byte   2                       ; Number of drawing colors
        .byte   0                       ; Number of screens available
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



; ------------------------------------------------------------------------
; Data.

; Variables mapped to the zero page segment variables. Some of these are
; used for passing parameters to the driver.

X1              = ptr1
Y1              = ptr2
X2              = ptr3
Y2              = ptr4

ADDR            = tmp1          ; ADDR+1,2,3

TEMP            = tmp3
TEMP2           = tmp4          ; HORLINE
TEMP3           = sreg          ; HORLINE


; Absolute variables used in the code

.bss

; The colors are indicies into a TGI palette.  The TGI palette is indicies into
; VERA's palette.  Vera's palette is a table of Red, Green, and Blue levels.
; The first 16 RGB elements mimic the Commodore 64's colors.

SCRBASE:        .res    1           ; High byte of screen base
BITMASK:        .res    1           ; $00 = clear, $FF = set pixels

defpalette:     .res    2
palette:        .res    2

color:          .res    1           ; Stroke and fill index
text_mode:      .res    1           ; Old text mode

tempX:          .res    2
tempY:          .res    2
ERR2:           .res    1
ERR:            .res    1
SY:             .res    1
SX:             .res    1
DY:             .res    1
DX:             .res    1
CURRENT_Y:      .res    2
CURRENT_X:      .res    2

.data

ERROR:          .byte   TGI_ERR_OK  ; Error code


; Constants and tables

.rodata

veracolors:
col_black:  .byte %00000000, %00000000
col_white:  .byte %11111111, %00001111
col_red:    .byte %00000000, %00001000
col_cyan:   .byte %11111110, %00001010
col_purple: .byte %01001100, %00001100
col_green:  .byte %11000101, %00000000
col_blue:   .byte %00001010, %00000000
col_yellow: .byte %11100111, %00001110
col_orange: .byte %10000101, %00001101
col_brown:  .byte %01000000, %00000110
col_lred:   .byte %01110111, %00001111
col_gray1:  .byte %00110011, %00000011
col_gray2:  .byte %01110111, %00000111
col_lgreen: .byte %11110110, %00001010
col_lblue:  .byte %10001111, %00000000
col_gray3:  .byte %10111011, %00001011

; Bit masks for setting pixels
bitMasks1:
    .byte %10000000, %01000000, %00100000, %00010000
    .byte %00001000, %00000100, %00000010, %00000001
bitMasks2:
    .byte %01111111, %10111111, %11011111, %11101111
    .byte %11110111, %11111011, %11111101, %11111110


.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. May
; initialize anything that has to be done just once. Is probably empty
; most of the time.
;
; Must set an error code: NO

INSTALL:
; Create the default palette.
        lda #$00
        sta defpalette
        lda #$01
        sta defpalette+1

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

INIT:   stz     ERROR           ; #TGI_ERR_OK

; Save the current text mode.

        sec
        jsr     SCREEN_MODE
        sta     text_mode

; Switch into (640 x 480 x 2 bpp) graphics mode.

        lda     #%00000000          ; DCSEL = 0, VRAM port 1
        sta     VERA::CTRL
        lda     #%00100001          ; Disable sprites, layer 1 enable, VGA
        sta     VERA::DISP::VIDEO
        lda     #%00000100          ; Bitmap mode enable
        sta     VERA::L1::CONFIG
        lda     #%00000001          ; Tile width 640
        sta     VERA::L1::TILE_BASE
        rts

; ------------------------------------------------------------------------
; DONE: Will be called to switch the graphics device back into text mode.
; The graphics kernel never will call DONE when no graphics mode is active,
; so there is no need to protect against that.
;
; Must set an error code: NO

DONE:
        jsr     CINT
        lda     text_mode
        clc
        jmp     SCREEN_MODE

; ------------------------------------------------------------------------
; GETERROR: Return the error code in .A, and clear it.

GETERROR:
        lda     ERROR
        stz     ERROR
        rts

; ------------------------------------------------------------------------
; CONTROL: Platform-/driver-specific entry point.
;
; Must set an error code: YES

CONTROL:
        lda     #TGI_ERR_INV_FUNC
        sta     ERROR
        rts

; ------------------------------------------------------------------------
; CLEAR: Clear the screen.
;
; Must set an error code: NO

CLEAR:
        .scope inner

        ; set up DCSEL=2
        lda     #(2 << 1)
        sta     VERA::CTRL

        ; set cache writes
        lda     #$40
        tsb     VERA::DISP::VIDEO        ; VERA_FX_CTRL when DCSEL=2

        ; set FX cache to all zeroes
        lda     #(6 << 1)
        sta     VERA::CTRL

        lda     #$00
        sta     VERA::DISP::VIDEO
        sta     VERA::DISP::HSCALE
        sta     VERA::DISP::VSCALE
        sta     VERA::DISP::FRAME

        stz     VERA::CTRL
        ; set address and increment for bitmap area
        stz     VERA::ADDR
        stz     VERA::ADDR + 1
        lda     #$30                    ; increment +4
        sta     VERA::ADDR + 2

        ldy     #$F0
@blank_outer:
        ldx     #$0A
@blank_loop:

        .repeat 8
        stz VERA::DATA0
        .endrep

        dex
        bne     @blank_loop
        dey
        bne     @blank_outer

        ; set up DCSEL=2
        lda     #(2 << 1)
        sta     VERA::CTRL

        ; set FX off (cache write bit 1 -> 0)
        stz     VERA::DISP::VIDEO        ; VERA_FX_CTRL when DCSEL=2
        stz     VERA::CTRL

        .endscope
        rts


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
        stz     ERROR           ; #TGI_ERR_OK
        ldy     #$01            ; Palette size of 2 colors
@L1:    lda     (ptr1),y        ; Copy the palette
        sta     palette,y
        dey
        bpl     @L1

        ; set background color from palette color 0
        lda     #$00
        sta     VERA::ADDR
        lda     #$FA
        sta     VERA::ADDR+1
        lda     #$01
        sta     VERA::ADDR+2    ; write color RAM @ $1FA00

        lda     palette
        asl
        tay
        lda     veracolors,y
        sta     VERA::DATA0

        inc     VERA::ADDR      ; $1FA01

        lda     palette
        asl
        tay
        iny     ; second byte of color
        lda     veracolors,y
        sta     VERA::DATA0

        ; set foreground color from palette color 1
        inc     VERA::ADDR      ; $1FA02

        lda     palette+1
        asl
        tay
        lda     veracolors,y
        sta     VERA::DATA0

        inc     VERA::ADDR      ; $1FA03

        lda     palette+1
        asl
        tay
        iny     ; second byte of color
        lda     veracolors,y
        sta     VERA::DATA0
        rts

; ------------------------------------------------------------------------
; SETCOLOR: Set the drawing color (in .A). The new color already is checked
; to be in a valid range (0..maxcolor).
;
; Must set an error code: NO (will be called only if color OK)

SETCOLOR:
        tax
        beq     @L1
        lda     #$FF
@L1:    sta     BITMASK
        stx     color
        rts

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
        jsr CALC

        stx TEMP

        lda ADDR
        ldy ADDR+1
        ldx #$00

        sta VERA::ADDR
        sty VERA::ADDR + 1
        stx VERA::ADDR + 2

        ldx TEMP

        lda BITMASK
        beq @ahead

        ; if BITMASK = $00, white is line color
        ; Set the bit in the byte at VERA_DATA0
        lda VERA::DATA0      ; Load the byte at memory address
        ora bitMasks1,X      ; OR with the bit mask
        sta VERA::DATA0      ; Store back the modified byte
        rts

@ahead:
        ; if BITMASK = $FF, black is line color
        lda VERA::DATA0      ; Load the byte at memory address
        and bitMasks2,X      ; OR with the bit mask
        sta VERA::DATA0      ; Store back the modified byte
        rts

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel, and return it in .XA. The
; co-ordinates passed to this function never are outside the visible screen
; area, so there is no need for clipping inside this function.

GETPIXEL:
        jsr CALC

        stx TEMP

        lda ADDR
        ldy ADDR+1
        ldx #$00

        sta VERA::ADDR
        sty VERA::ADDR + 1
        stx VERA::ADDR + 2

        ldx TEMP
        lda VERA::DATA0      ; Load the byte at memory address
        and bitMasks1,X

        bne @ahead

        ldx #$00
        lda #$00
        rts

@ahead:
        ldx #$00
        lda #$01
        rts

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
        ; Initialize tempY with Y1
        lda Y1
        sta tempY
        lda Y1+1
        sta tempY+1

@outer_loop:
        ; Compare tempY with Y2
        lda tempY+1
        cmp Y2+1
        bcc @outer_continue   ; If tempY high byte < Y2 high byte, continue
        bne @outer_end        ; If tempY high byte > Y2 high byte, end
        lda tempY
        cmp Y2
        bcc @outer_continue   ; If tempY low byte < Y2 low byte, continue
        beq @outer_end        ; If tempY low byte = Y2 low byte, end

@outer_continue:
        ; Initialize tempX with X1
        lda X1
        sta tempX
        lda X1+1
        sta tempX+1

@inner_loop:
        ; Compare tempX with X2
        lda tempX+1
        cmp X2+1
        bcc @inner_continue   ; If tempX high byte < X2 high byte, continue
        bne @inner_end        ; If tempX high byte > X2 high byte, end
        lda tempX
        cmp X2
        bcc @inner_continue   ; If tempX low byte < X2 low byte, continue

@inner_end:
        ; Increment tempY
        inc tempY
        bne @outer_loop       ; If no overflow, continue outer loop
        inc tempY+1           ; If overflow, increment high byte

@inner_continue:
        ; Call setpixel(tempX, tempY)
        lda X1
        pha
        lda X1+1
        pha
        lda Y1
        pha
        lda Y1+1
        pha

        lda tempX
        ldx tempX+1
        sta X1
        stx X1+1

        lda tempY
        ldx tempY+1
        sta Y1
        stx Y1+1

        jsr SETPIXEL

        pla
        sta Y1+1
        pla
        sta Y1
        pla
        sta X1+1
        pla
        sta X1

    ; Increment tempX
        inc tempX
        bne @inner_loop_check ; If no overflow, continue
        inc tempX+1           ; If overflow, increment high byte

@inner_loop_check:
        ; Compare tempX with X2 again after increment
        lda tempX+1
        cmp X2+1
        bcc @inner_continue   ; If tempX high byte < X2 high byte, continue
        bne @outer_increment  ; If tempX high byte > X2 high byte, increment tempY
        lda tempX
        cmp X2
        bcc @inner_continue   ; If tempX low byte < X2 low byte, continue

@outer_increment:
        ; Increment tempY
        inc tempY
        bne @outer_loop       ; If no overflow, continue outer loop
        inc tempY+1           ; If overflow, increment high byte

@outer_end:
        jmp @done

@done:
        rts

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
        rts


; ------------------------------------------------------------------------
; Calculate all variables to plot the pixel at X1/Y1.
;------------------------
;< X1,Y1 - pixel
;> ADDR - address of card
;> X - bit number (X1 & 7)
CALC:
        lda     Y1+1
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
        lda     ADDR+1
        lda     X1
        and     #7
        tax
        rts


.include        "../../tgi/tgidrv_line.inc"
