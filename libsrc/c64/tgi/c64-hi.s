;
; Graphics driver for the 320x200x2 mode on the C64.
;
; Based on Stephen L. Judds GRLIB code
;

        .include        "zeropage.inc"

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table and constants.

        module_header   _c64_hi_tgi

; First part of the header is a structure that has a magic and defines the
; capabilities of the driver

        .byte   $74, $67, $69           ; "tgi"
        .byte   TGI_API_VERSION         ; TGI API version number
        .addr   $0000                   ; Library reference
        .word   320                     ; X resolution
        .word   200                     ; Y resolution
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

CHARROM         := $D000                ; Character rom base address
CBASE           := $D000                ; Color memory base address
VBASE           := $E000                ; Video memory base address


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

; Switch into graphics mode

        lda     $DD02           ; Set the data direction regs
        ora     #3
        sta     $DD02
        lda     $DD00
        and     #$FC            ; Switch to bank 3
        sta     $DD00

        lda     $D018
        sta     OLDD018
        lda     #$48            ; Set color map to $D000, screen to $E000
        sta     $D018

        lda     $D011           ; And turn on bitmap
        ora     #$20
DONE1:  sta     $D011

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

DONE:   lda     $DD02           ; Set the data direction regs
        ora     #3
        sta     $DD02
        lda     $DD00
        ora     #$03            ; Bank 0
        sta     $DD00

        lda     OLDD018         ; Screen mem --> $0400
        sta     $D018

        lda     $D011
        and     #<~$20
        sta     $D011
        rts

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
        tya
@L1:    sta     VBASE+$0000,y
        sta     VBASE+$0100,y
        sta     VBASE+$0200,y
        sta     VBASE+$0300,y
        sta     VBASE+$0400,y
        sta     VBASE+$0500,y
        sta     VBASE+$0600,y
        sta     VBASE+$0700,y
        sta     VBASE+$0800,y
        sta     VBASE+$0900,y
        sta     VBASE+$0A00,y
        sta     VBASE+$0B00,y
        sta     VBASE+$0C00,y
        sta     VBASE+$0D00,y
        sta     VBASE+$0E00,y
        sta     VBASE+$0F00,y
        sta     VBASE+$1000,y
        sta     VBASE+$1100,y
        sta     VBASE+$1200,y
        sta     VBASE+$1300,y
        sta     VBASE+$1400,y
        sta     VBASE+$1500,y
        sta     VBASE+$1600,y
        sta     VBASE+$1700,y
        sta     VBASE+$1800,y
        sta     VBASE+$1900,y
        sta     VBASE+$1A00,y
        sta     VBASE+$1B00,y
        sta     VBASE+$1C00,y
        sta     VBASE+$1D00,y
        sta     VBASE+$1E00,y
        sta     VBASE+$1F00,y
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

; Get the color entries from the palette

        lda     PALETTE+1       ; Foreground color
        asl     a
        asl     a
        asl     a
        asl     a
        ora     PALETTE         ; Background color
        tax

; Initialize the color map with the new color settings (it is below the
; I/O area)

        ldy     #$00
        sei
        lda     $01             ; Get ROM config
        pha                     ; Save it
        and     #%11111100      ; Clear bit 0 and 1
        sta     $01
        txa                     ; Load color code
@L2:    sta     CBASE+$0000,y
        sta     CBASE+$0100,y
        sta     CBASE+$0200,y
        sta     CBASE+$0300,y
        iny
        bne     @L2
        pla
        sta     $01
        cli

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

        sei                     ; Get underneath ROM
        lda     $01
        pha
        lda     #$34
        sta     $01

        lda     (POINT),Y
        eor     BITMASK
        and     BITTAB,X
        eor     (POINT),Y
        sta     (POINT),Y

        pla
        sta     $01
        cli

@L9:    rts

; ------------------------------------------------------------------------
; GETPIXEL: Read the color value of a pixel and return it in A/X. The
; coordinates passed to this function are never outside the visible screen
; area, so there is no need for clipping inside this function.


GETPIXEL:
        jsr     CALC            ; Calculate coordinates

        sei                     ; Get underneath ROM
        lda     $01
        pha
        lda     #$34
        sta     $01

        lda     (POINT),Y
        ldy     #$00
        and     BITTAB,X
        beq     @L1
        iny

@L1:    pla
        sta     $01
        cli

        tya                     ; Get color value into A
        ldx     #$00            ; Clear high byte
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

@CHECK: lda     X2           ;Make sure x1<x2
        sec
        sbc     X1
        tax
        lda     X2+1
        sbc     X1+1
        bpl     @CONT
        lda     Y2           ;If not, swap P1 and P2
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

        ldx     #$C8         ;INY
        lda     Y2           ;Calculate dy
        sec
        sbc     Y1
        tay
        lda     Y2+1
        sbc     Y1+1
        bpl     @DYPOS       ;Is y2>=y1?
        lda     Y1           ;Otherwise dy=y1-y2
        sec
        sbc     Y2
        tay
        ldx     #$88         ;DEY

@DYPOS: sty     DY              ; 8-bit DY -- FIX ME?
        stx     YINCDEC
        stx     XINCDEC

        jsr     CALC            ; Set up .X,.Y,POINT, and INRANGE
        lda     BITCHUNK,X
        sta     OLDCHUNK
        sta     CHUNK

        sei                     ; Get underneath ROM
        lda     #$34
        sta     $01

        ldx     DY
        cpx     DX           ;Who's bigger: dy or dx?
        bcc     STEPINX      ;If dx, then...
        lda     DX+1
        bne     STEPINX

;
; Big steps in Y
;
;   To simplify my life, just use PLOT to plot points.
;
;   No more!
;   Added special plotting routine -- cool!
;
;   X is now counter, Y is y-coordinate
;
; On entry, X=DY=number of loop iterations, and Y=
;   Y1 AND #$07
STEPINY:
        lda     #00
        sta     OLDCHUNK     ;So plotting routine will work right
        lda     CHUNK
        lsr                  ;Strip the bit
        eor     CHUNK
        sta     CHUNK
        txa
        bne     @CONT        ;If dy=0 it's just a point
        inx
@CONT:  lsr                  ;Init counter to dy/2
;
; Main loop
;
YLOOP:  sta     TEMP

        lda     INRANGE      ;Range check
        bne     @SKIP

        lda     (POINT),y    ;Otherwise plot
        eor     BITMASK
        and     CHUNK
        eor     (POINT),y
        sta     (POINT),y
@SKIP:
YINCDEC:
        iny                  ;Advance Y coordinate
        cpy     #8
        bcc     @CONT        ;No prob if Y=0..7
        jsr     FIXY
@CONT:  lda     TEMP         ;Restore A
        sec
        sbc     DX
        bcc     YFIXX
YCONT:  dex                  ;X is counter
        bne     YLOOP
YCONT2: lda     (POINT),y    ;Plot endpoint
        eor     BITMASK
        and     CHUNK
        eor     (POINT),y
        sta     (POINT),y
YDONE:  lda     #$36
        sta     $01
        cli
        rts

YFIXX:                    ;x=x+1
        adc     DY
        lsr     CHUNK
        bne     YCONT        ;If we pass a column boundary...
        ror     CHUNK        ;then reset CHUNK to $80
        sta     TEMP2
        lda     COL
        bmi     @C1          ;Skip if column is negative
        cmp     #39          ;End if move past end of screen
        bcs     YDONE
@C1:    lda     POINT        ;And add 8 to POINT
        adc     #8
        sta     POINT
        bcc     @CONT
        inc     POINT+1
@CONT:  inc     COL          ;Increment column
        bne     @C2
        lda     ROW          ;Range check
        cmp     #25
        bcs     @C2
        lda     #00          ;Passed into col 0
        sta     INRANGE
@C2:    lda     TEMP2
        dex
        bne     YLOOP
        beq     YCONT2

;
; Big steps in X direction
;
; On entry, X=DY=number of loop iterations, and Y=
;   Y1 AND #$07

.bss
COUNTHI:
        .byte   $00       ;Temporary counter
                          ;only used once
.code
STEPINX:
        ldx     DX
        lda     DX+1
        sta     COUNTHI
        cmp     #$80
        ror                  ;Need bit for initialization
        sta     Y1           ;High byte of counter
        txa
        bne     @CONT        ;Could be $100
        dec     COUNTHI
@CONT:  ror
;
; Main loop
;
XLOOP:  lsr     CHUNK
        beq     XFIXC        ;If we pass a column boundary...
XCONT1: sbc     DY
        bcc     XFIXY        ;Time to step in Y?
XCONT2: dex
        bne     XLOOP
        dec     COUNTHI      ;High bits set?
        bpl     XLOOP

XDONE:  lsr     CHUNK        ;Advance to last point
        jsr     LINEPLOT     ;Plot the last chunk
EXIT:   lda     #$36
        sta     $01
        cli
        rts
;
; CHUNK has passed a column, so plot and increment pointer
; and fix up CHUNK, OLDCHUNK.
;
XFIXC:  sta     TEMP
        jsr     LINEPLOT
        lda     #$FF
        sta     CHUNK
        sta     OLDCHUNK
        lda     COL
        bmi     @C1          ;Skip if column is negative
        cmp     #39          ;End if move past end of screen
        bcs     EXIT
@C1:    lda     POINT
        adc     #8
        sta     POINT
        bcc     @CONT
        inc     POINT+1
@CONT:  inc     COL
        bne     @C2
        lda     ROW
        cmp     #25
        bcs     @C2
        lda     #00
        sta     INRANGE
@C2:    lda     TEMP
        sec
        bcs     XCONT1
;
; Check to make sure there isn't a high bit, plot chunk,
; and update Y-coordinate.
;
XFIXY:  dec     Y1           ;Maybe high bit set
        bpl     XCONT2
        adc     DX
        sta     TEMP
        lda     DX+1
        adc     #$FF         ;Hi byte
        sta     Y1

        jsr     LINEPLOT     ;Plot chunk
        lda     CHUNK
        sta     OLDCHUNK

        lda     TEMP
XINCDEC:
        iny                  ;Y-coord
        cpy     #8           ;0..7 is ok
        bcc     XCONT2
        sta     TEMP
        jsr     FIXY
        lda     TEMP
        jmp     XCONT2

;
; Subroutine to plot chunks/points (to save a little
; room, gray hair, etc.)
;
LINEPLOT:                       ; Plot the line chunk
        lda     INRANGE
        bne     @SKIP

        lda     (POINT),Y       ; Otherwise plot
        eor     BITMASK
        ora     CHUNK
        and     OLDCHUNK
        eor     CHUNK
        eor     (POINT),Y
        sta     (POINT),Y
@SKIP:  rts

;
; Subroutine to fix up pointer when Y decreases through
; zero or increases through 7.
;
FIXY:   cpy     #255         ;Y=255 or Y=8
        beq     @DECPTR
@INCPTR:                     ;Add 320 to pointer
        ldy     #0           ;Y increased through 7
        lda     ROW
        bmi     @C1          ;If negative, then don't update
        cmp     #24
        bcs     @TOAST       ;If at bottom of screen then quit
@C1:    lda     POINT
        adc     #<320
        sta     POINT
        lda     POINT+1
        adc     #>320
        sta     POINT+1
@CONT1: inc     ROW
        bne     @DONE
        lda     COL
        bpl     @CLEAR
@DONE:  rts

@DECPTR:                     ;Okay, subtract 320 then
        ldy     #7           ;Y decreased through 0
        lda     POINT
        sec
        sbc     #<320
        sta     POINT
        lda     POINT+1
        sbc     #>320
        sta     POINT+1
@CONT2: dec     ROW
        bmi     @TOAST
        lda     ROW
        cmp     #24
        bne     @DONE
        lda     COL
        bmi     @DONE
@CLEAR: lda     #00
        sta     INRANGE
        rts

@TOAST: pla                  ;Remove old return address
        pla
        jmp     EXIT         ;Restore interrupts, etc.

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
; of range, a carry is returned and INRANGE is set to a value !0 zero. If
; the coordinates are valid, INRANGE is zero and the carry clear.

CALC:   lda     Y1
        sta     ROW
        and     #7
        tay
        lda     Y1+1
        lsr                     ; Neg is possible
        ror     ROW
        lsr
        ror     ROW
        lsr
        ror     ROW

        lda     #00
        sta     POINT
        lda     ROW
        cmp     #$80
        ror
        ror     POINT
        cmp     #$80
        ror
        ror     POINT           ; row*64
        adc     ROW             ; +row*256
        clc
        adc     #>VBASE         ; +bitmap base
        sta     POINT+1

        lda     X1
        tax
        sta     COL
        lda     X1+1
        lsr
        ror     COL
        lsr
        ror     COL
        lsr
        ror     COL

        txa
        and     #$F8
        clc
        adc     POINT           ; +(X AND #$F8)
        sta     POINT
        lda     X1+1
        adc     POINT+1
        sta     POINT+1
        txa
        and     #7
        tax

        lda     ROW
        cmp     #25
        bcs     @L9
        lda     COL
        cmp     #40
        bcs     @L9
        lda     #00
@L9:    sta     INRANGE
        rts


