;
; Generic Atari graphics driver
;

.macpack longbranch
.macpack module

.ifdef __ATARIXL__
        CIO_vec := my_CIOV
.else
        CIO_vec := CIOV
.endif

; ******************************************************************************

        ; ----------------------------------------------------------------------
        ;
        ; Header. Includes jump table and constants.
        ;
        ; ----------------------------------------------------------------------

.ifdef __ATARIXL__
        .define LABEL_X "x"
.else
        .define LABEL_X ""
.endif

.if pages = 2
        .define LABEL_P2 "p2"
.else
        .define LABEL_P2 ""
.endif

        module_header   .ident (.sprintf ("_atr%s%d%s_tgi", LABEL_X, ::grmode, LABEL_P2))

; Header

        .byte   $74, $67, $69           ; "tgi"
        .byte   TGI_API_VERSION         ; TGI API version number
libref: .addr   $0000                   ; Library reference
        .word   x_res                   ; X resolution
        .word   y_res                   ; Y resolution
        .byte   colors                  ; Number of drawing colors
        .byte   pages                   ; Number of screens available
        .byte   8                       ; System font X size
        .byte   8                       ; System font Y size
        .word   aspect                  ; Aspect ratio
        .byte   0                       ; TGI driver flags

; Function table

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

; ******************************************************************************

        ; ----------------------------------------------------------------------
        ;
        ; Parameters
        ;
        ; ----------------------------------------------------------------------

        x1      := ptr1
        y1      := ptr2
        x2      := ptr3
        y2      := ptr4
        radius  := tmp1

; ******************************************************************************

        ; ----------------------------------------------------------------------
        ;
        ; Global variables
        ;
        ; ----------------------------------------------------------------------

        sptr    := regsave + 2

.bss
        error:
                .res    1       ; Error code

.if ::grmode = 9 || ::grmode = 11
        palette = default_palette
.else
        palette:
                .res    colors  ; The current palette
.endif

        mask:
                .res    1       ; Current pixel mask
        griocb:
                .res    1       ; IOCB channel number for graphics

.if pages = 2
        p0scr:
                .res    1       ; High byte of screen address for screen page 0
        p0dls:
                .res    1       ; High byte of display list address for screen page 0
                                ; Page 1's addresses are 8K higher
.endif

.data
        mag_x:
                .byte   1       ; Horizontal text scaling factor
        mag_y:
                .byte   1       ; Vertical text scaling factor
        mag_x8:
                .word   8       ; Horizontal text scaling factor * 8
        mag_y8:
                .word   8       ; Vertical text scaling factor * 8
        text_dir:
                .byte   0       ; Text direction,

.ifdef __ATARIXL__
        my_CIOV:
                .byte   $4C, 0, 0
.endif
.code

; ******************************************************************************

.macro  put_pixel

        ; ----------------------------------------------------------------------
        ;
        ; Put a pixel at (sptr),y using x as the bit mask offset
        ;
        ; ----------------------------------------------------------------------

        lda     (sptr),y
        eor     mask
        and     mask_table,x
        eor     (sptr),y
        sta     (sptr),y
.endmacro

; ******************************************************************************

.rodata
screen_device:
        .byte "S:",$9B          ; Device code for screen
screen_device_length := * - screen_device

.code

.proc INIT

        ; ----------------------------------------------------------------------
        ;
        ; INIT: Switch to graphics mode
        ;
        ; ----------------------------------------------------------------------


.code
        ; Initialize drawing color

.if ::ppb = 8
        ldx     #$FF
.elseif ::ppb = 4
        ldx     #$55
.elseif ::ppb = 2
        ldx     #$11
.endif

        stx     mask

.ifdef __ATARIXL__

        ; setup pointer to CIO

        lda     libref
        sta     my_CIOV+1
        lda     libref+1
        sta     my_CIOV+2
.endif


        ; Find a free IOCB
        lda     #$70
search: tax
        ldy     ICHID,x
        cpy     #$FF
        beq     found
        sub     #$10
        bcs     search

        ; Not enough resources available (free IOCB or memory)
        ; enter with C cleared!
nores:  lda     #TGI_ERR_NO_RES
        bcc     exit

found:  ; Check if enough RAM is available
        lda     #0
        sub     #<mem_needed
        tay
        lda     RAMTOP
        sbc     #>mem_needed
        cmp     APPMHI + 1
        bcc     nores
        bne     switch
        cpy     APPMHI
        bcc     nores                   ; not enough memory

        ; Switch into graphics mode
switch: lda     #OPEN
        sta     ICCOM,x
        lda     #OPNIN | OPNOT
        sta     ICAX1,x
        lda     #::grmode
        sta     ICAX2,x
        lda     #<screen_device
        sta     ICBAL,x
        lda     #>screen_device
        sta     ICBAH,x
        lda     #<screen_device_length
        sta     ICBLL,x
        lda     #>screen_device_length
        sta     ICBLH,x
        jsr     CIO_vec

.if ::pages = 2
        ; Reserve 8K of high memory
        lda     RAMTOP
        sub     #32
        sta     RAMTOP
        ; Close and reopen graphics
        lda     #CLOSE
        sta     ICCOM,x
        jsr     CIO_vec
        ; Reopen graphics
        lda     #OPEN
        sta     ICCOM,x
        lda     #OPNIN | OPNOT
        sta     ICAX1,x
        lda     #::grmode
        sta     ICAX2,x
        lda     #<screen_device
        sta     ICBAL,x
        lda     #>screen_device
        sta     ICBAH,x
        lda     #<screen_device_length
        sta     ICBLL,x
        lda     #>screen_device_length
        sta     ICBLH,x
        jsr     CIO_vec
        ; Save screen pointers
        lda     SAVMSC + 1
        sta     p0scr
        lda     SDLSTH
        sta     p0dls
.endif ; ::pages = 2

        stx     griocb

        ; Reset the error code and return
        lda     #TGI_ERR_OK
exit:   sta     error
        rts
.endproc

; ******************************************************************************

.proc DONE

        ; ----------------------------------------------------------------------
        ;
        ; DONE: Switch back to text mode
        ;
        ; ----------------------------------------------------------------------

.code

.if ::pages = 2
        ; Free 8K of high memory
        lda     RAMTOP
        add     #32
        sta     RAMTOP
.endif

        ; Clear griocb
        lda     #$FF
        ldx     griocb
        sta     griocb

        ; Close the S: device
        lda     #CLOSE
        sta     ICCOM,x
        jsr     CIO_vec

        ; Reopen it in Graphics 0
        lda     #OPEN
        sta     ICCOM,x
        lda     #OPNIN | OPNOT
        sta     ICAX1,x
        lda     #0
        sta     ICAX2,x
        lda     #<screen_device
        sta     ICBAL,x
        lda     #>screen_device
        sta     ICBAH,x
        lda     #<screen_device_length
        sta     ICBLL,x
        lda     #>screen_device_length
        sta     ICBLH,x
        jsr     CIO_vec

        ; Now close it again; we don't need it anymore :)
        lda     #CLOSE
        sta     ICCOM,x
        jmp     CIO_vec
.endproc

; ******************************************************************************

.proc GETERROR

        ; ----------------------------------------------------------------------
        ;
        ; GETERROR: Return the error code in A and clear it
        ;
        ; ----------------------------------------------------------------------

.code
        ldx     #TGI_ERR_OK
        lda     error
        stx     error
        rts
.endproc

; ******************************************************************************

.proc CLEAR

        ; ----------------------------------------------------------------------
        ;
        ; CLEAR: Clear the screen
        ;
        ; ----------------------------------------------------------------------

.code
        ; Load the screen address in sptr
        lda     SAVMSC
        sta     sptr
        lda     SAVMSC + 1
        sta     sptr + 1

        ; Fill with zero
        lda     #0
        tay

.if >::scrsize > 0
        ; Clear full pages if any
        ldx     #>::scrsize
loop1:  sta     (sptr),y
        iny
        bne     loop1
        inc     sptr + 1
        dex
        bne     loop1
.endif

.if <::scrsize > 0
        ; Clear the rest, if any
loop2:  sta     (sptr),y
        iny
        cpy     #<::scrsize
        bne     loop2
.endif

        rts
.endproc

; ******************************************************************************

.proc GETPALETTE

        ; ----------------------------------------------------------------------
        ;
        ; GETPALETTE: Return the current palette in A/X
        ;
        ; ----------------------------------------------------------------------

.code
        lda     #<palette
        ldx     #>palette
        rts
.endproc

; ******************************************************************************

.proc GETDEFPALETTE

        ; ----------------------------------------------------------------------
        ;
        ; GETDEFPALETTE: Return the default palette in A/X
        ;
        ; ----------------------------------------------------------------------

.code
        lda     #<default_palette
        ldx     #>default_palette
        rts
.endproc

; ******************************************************************************

.proc SETCOLOR

        ; ----------------------------------------------------------------------
        ;
        ; SETCOLOR: Set the drawing color (in A)
        ;
        ; ----------------------------------------------------------------------

.code
        tax

.if ::grmode = 9
        ; Map colors like this: 0 -> 0, 1 -> 15, 2 -> 1, 3 -> 2 etc.
        beq     cont
        cpx     #1
        bne     map
        ldx     #16
map:    dex
cont:
.endif

        lda     masks,x
        sta     mask
        rts
.endproc

; ******************************************************************************

.proc CALC

        ; ----------------------------------------------------------------------
        ;
        ; CALC: Calculate the screen address
        ;       in
        ;               x1 (ptr1)       x coordinate
        ;               y1 (ptr2)       y coordinate
        ;       out
        ;               sptr + y        screen address
        ;               x               bit mask index
        ;
        ; ----------------------------------------------------------------------

.bss
        temp:   .res    1
.code
        ; calculate line offset
        lda     y1 + 1
        sta     temp
        lda     y1

.if ::x_res / ::ppb = 40
        yrep = 3
.elseif ::x_res / ::ppb = 20
        yrep = 2
.elseif ::x_res / ::ppb = 10
        yrep = 1
.endif

.repeat yrep
        asl     a
        rol     temp
.endrepeat

        sta     sptr
        ldx     temp
        stx     sptr + 1

.repeat 2
          asl   a
          rol   temp
.endrepeat

        add     sptr
        sta     sptr
        lda     temp
        adc     sptr + 1
        sta     sptr + 1

        ; calculate bit mask offset
        lda     x1
        and     #ppb - 1
        tax

        ; calculate row offset
        lda     x1 + 1
        sta     temp
        lda     x1

.if ::ppb = 8
        xrep = 3
.elseif ::ppb = 4
        xrep = 2
.elseif ::ppb = 2
        xrep = 1
.endif

.repeat xrep
        lsr     temp
        ror     a
.endrepeat

        tay

        ; sptr += SAVMSC
        lda     SAVMSC
        add     sptr
        sta     sptr
        lda     SAVMSC + 1
        adc     sptr + 1
        sta     sptr + 1

        ; We're done!
        rts
.endproc

; ******************************************************************************

.proc SETPIXEL

        ; ----------------------------------------------------------------------
        ;
        ; Draw one pixel at x1, y1
        ;
        ; ----------------------------------------------------------------------

.code
        jsr     CALC
        put_pixel
        rts
.endproc

; ******************************************************************************

.proc GETPIXEL

        ; ----------------------------------------------------------------------
        ;
        ; GETPIXEL: Read the color value of a pixel and return it in A/X
        ;
        ; ----------------------------------------------------------------------

.code
        jsr     CALC
        lda     (sptr),y
        and     mask_table,x

.if ::ppb = 8
        beq     zero
        lda     #1
zero:   ldx     #0
        rts
.elseif ::ppb = 4
loop:   cpx     #3
        beq     done4
        lsr     a
        lsr     a
        inx
        bne     loop
done4:  and     #$03
        ldx     #0
        rts
.elseif ::ppb = 2
        dex
        bne     shift
        and     #$0F
        jmp     exit
shift:  lsr     a
        lsr     a
        lsr     a
        lsr     a

exit:

.if ::grmode = 9
        ; Mode 9 mapping
        ; Map colors like this: 0 -> 0, 15 -> 1, 2 -> 3, 3 -> 4 etc.
        beq     done9
        cmp     #15
        bne     map9
        lda     #0
map9:   add     #1
done9:
.endif

.if ::grmode = 10
        ; Mode 10 mapping
        ; Map out of range colors like this:
        ;  9 -> 8
        ; 10 -> 8
        ; 11 -> 8
        ; 12 -> 0
        ; 13 -> 1
        ; 14 -> 2
        ; 15 -> 3
        cmp     #9
        bcc     done10
        sub     #12
        bcs     done10
        lda     #8
done10:
.endif ; ::grmode = 10

        ; Done!
        ldx     #0
        rts
.endif  ; ::ppb = 2

.endproc

; ******************************************************************************

.proc LINE

        ; ----------------------------------------------------------------------
        ;
        ; LINE: Draw a line from x1,y1 to x2,y2
        ;
        ; ----------------------------------------------------------------------

.ifdef USE_CIO_LINE

        ; position ptr1, ptr2
        lda     x1
        sta     OLDCOL
        lda     x1 + 1
        sta     OLDCOL + 1
        lda     y1
        sta     OLDROW
        ; plot
        jsr     SETPIXEL
        ; position ptr3,ptr4
        lda     x2
        sta     COLCRS
        lda     x2 + 1
        sta     COLCRS + 1
        lda     y2
        sta     ROWCRS
        ; drawto
        ldx     griocb
        lda     #DRAWLN
        sta     ICCOM,x
        lda     mask

.if ::grmode = 10
        and     #$0f
.else
        and     #colors - 1
.endif

        sta     ATACHR
        jmp     CIO_vec

.else   ; USE_CIO_LINE

; locals

        dx      :=      sreg
        dy      :=      y1
        dx2     :=      x2
        dy2     :=      y2
        iy      :=      tmp1
        err     :=      tmp3

.code
        ; dx = x2 - x1
        lda     x2
        sub     x1
        sta     dx
        lda     x2 + 1
        sbc     x1 + 1
        sta     dx + 1
        ; if dx is positive, no problem
        bcs     dx_positive

        ; if dx is negative, swap x1,y1 with x2,y2
        lda     x1      ; x1 <-> x2, low byte
        ldx     x2
        sta     x2
        stx     x1
        lda     x1 + 1  ; x1 <-> x2, high byte
        ldx     x2 + 1
        sta     x2 + 1
        stx     x1 + 1
        lda     y1      ; y1 <-> y2, low byte
        ldx     y2
        sta     y2
        stx     y1
        lda     y1 + 1  ; y1 <-> y2, high byte
        ldx     y2 + 1
        sta     y2 + 1
        stx     y1 + 1
        ; Calculate again
        jmp     LINE

dx_positive:
        ; Calculate coords
        jsr     CALC

        ; dy = y2 - y1
        lda     y2
        sub     y1
        sta     dy
        lda     y2 + 1
        sbc     y1 + 1
        sta     dy + 1

        ; if dy is negative
        bcs     dy_positive
        ; dy = -dy
        lda     #0
        sub     dy
        sta     dy
        lda     #0
        sbc     dy + 1
        sta     dy + 1
        ; iy = -row_size
        lda     #<(65536 - x_res / ppb)
        sta     iy
        lda     #>(65536 - x_res / ppb)
        sta     iy + 1
        bne     skip_iy_1               ; always

dy_positive:
        ; iy = row_size
        lda     #<(x_res / ppb)
        sta     iy
        lda     #>(x_res / ppb)
        sta     iy + 1
skip_iy_1:

        ; dx2 = dx * 2
        lda     dx
        asl     a
        sta     dx2
        lda     dx + 1
        rol     a
        sta     dx2 + 1

        ; dy2 = dy * 2
        lda     dy
        asl     a
        sta     dy2
        lda     dy + 1
        rol     a
        sta     dy2 + 1

        ; if dx >= dy
        lda     dx
        cmp     dy
        lda     dx + 1
        sbc     dy + 1
        bcc     dy_major

        ; dx is the major axis

        ; err = dy2 - dx
        lda     dy2
        sub     dx
        sta     err
        lda     dy2 + 1
        sbc     dx + 1
        sta     err + 1

        .scope
loop:   ; main loop
        put_pixel
        ; if err >= 0
        lda     err + 1
        bmi     err_neg
        ; err -= dx2
        lda     err
        sub     dx2
        sta     err
        lda     err + 1
        sbc     dx2 + 1
        sta     err + 1
        ; move_vertical (iy)
        lda     sptr
        add     iy
        sta     sptr
        lda     sptr + 1
        adc     iy + 1
        sta     sptr + 1
err_neg:
        ; err += dy2
        lda     err
        add     dy2
        sta     err
        lda     err + 1
        adc     dy2 + 1
        sta     err + 1
        ; move_right
        inx
        cpx     #ppb
        bne     end_move
        ldx     #0
        iny
        bne     end_move
        inc     sptr + 1
end_move:
        ; loop while dx-- >= 0
        lda     dx
        ora     dx + 1
        beq     exit
        dec     dx
        lda     dx
        cmp     #$FF
        bne     loop
        dec     dx + 1
        jmp     loop
exit:   rts
        .endscope

dy_major:
        ; dy is the major axis

        ; err = dx2 - dy;
        lda     dx2
        sub     dy
        sta     err
        lda     dx2 + 1
        sbc     dy + 1
        sta     err + 1

        .scope
loop:   ; main loop
        put_pixel
        ; if err >= 0
        lda     err + 1
        bmi     end_move
        ; err -= dy2
        lda     err
        sub     dy2
        sta     err
        lda     err + 1
        sbc     dy2 + 1
        sta     err + 1
        ; move_right
        inx
        cpx     #ppb
        bne     end_move
        ldx     #0
        iny
        bne     end_move
        inc     sptr + 1
end_move:
        ; err += dx2
        lda     err
        add     dx2
        sta     err
        lda     err + 1
        adc     dx2 + 1
        sta     err + 1
        ; move_vertical(iy)
        lda     sptr
        add     iy
        sta     sptr
        lda     sptr + 1
        adc     iy + 1
        sta     sptr + 1
        ; loop while dy-- >= 0
        lda     dy
        ora     dy + 1
        beq     exit
        dec     dy
        lda     dy
        cmp     #$FF
        bne     loop
        dec     dy + 1
        jmp     loop
exit:   rts
        .endscope
.endif  ; USE_CIO_LINE
.endproc

; ******************************************************************************

.proc clipped_bar

        ; ----------------------------------------------------------------------
        ;
        ; Clip and draw bar, this function will disappear when text clipping
        ; will be done int the TGI kernel
        ;
        ; ----------------------------------------------------------------------

.code
        lda     y1 + 1
        bne     off
        lda     y1
        cmp     #y_res
        bcs     off

        lda     x1 + 1

.if >(::x_res - 1) > 0
        cmp     #>x_res
        bcc     check2
.endif

        bne     off
        lda     x1
        cmp     #<x_res
        bcc     check2
off:    rts

check2: lda     y2 + 1
        bne     off
        lda     y2
        cmp     #y_res
        bcs     off

        lda     x2 + 1

.if >(::x_res - 1) > 0
        cmp     #>x_res
        bcc     BAR
.endif

        bne     off
        lda     x2
        cmp     #<x_res
        bcs     off
.endproc

; ******************************************************************************

.proc BAR

        ; ----------------------------------------------------------------------
        ;
        ; BAR: Draw a filled rectangle with the corners at x1,y1,x2,y2
        ;
        ; ----------------------------------------------------------------------

; locals
        lmem    := sreg
.bss
        lmask:  .res 1
        rmask:  .res 1
        dy:     .res 1
        dx:     .res 1
        fmask:  .res 1
.code
        ; dy = y2 - y1 + 1
        lda     y2
        sub     y1
        sta     dy
        inc     dy
        ; Calculate upper left corner
        jsr     CALC
        ; Save the values
        tya
        add     sptr
        sta     lmem
        lda     sptr + 1
        adc     #0
        sta     lmem + 1
        lda     bar_table,x
        sta     lmask
        ; Calculate upper right corner
        lda     x2
        sta     x1

.if >(::x_res - 1) > 0
        lda     x2 + 1
        sta     x1 + 1
.endif

        jsr     CALC
        ; Save the values
        tya
        add     sptr
        sta     sptr
        bcc     skips
        inc     sptr + 1
skips:  inx
        lda     bar_table,x
        eor     #$FF
        sta     rmask
        ; Calculate memory difference between x1 and x2
        lda     sptr
        sub     lmem
        sta     dx
loop:   ; Main loop
        ldy     #0
        ldx     dx
        beq     same
        ; Left
        lda     (lmem),y
        eor     mask
        and     lmask
        eor     (lmem),y
        sta     (lmem),y
        iny
        ; Between
        lda     mask
        jmp     next
btwn:   sta     (lmem),y
        iny
next:   dex
        bne     btwn
        ; Right
        lda     (lmem),y
        eor     mask
        and     rmask
        eor     (lmem),y
        sta     (lmem),y
        jmp     cont
same:   ; Same byte
        lda     lmask
        and     rmask
        sta     fmask
        lda     (lmem),y
        eor     mask
        and     fmask
        eor     (lmem),y
        sta     (lmem),y
cont:   ; Go to next row
        lda     lmem
        add     #<(x_res / ppb)
        sta     lmem
        bcc     skipm
        inc     lmem + 1
skipm:  ; Loop while --dy > 0
        dec     dy
        bne     loop

        rts

.endproc

; ******************************************************************************

.proc TEXTSTYLE

        ; ----------------------------------------------------------------------
        ;
        ; TEXTSTYLE: Set text style. Scale factors in X and Y and direction in A
        ;
        ; ----------------------------------------------------------------------

.code
        stx     mag_x
        sty     mag_y
        ; Save text direction in bit 8 so that we can use BIT instruction later
        lsr     a
        ror     a
        sta     text_dir
        ; Save 8 * scaling factors
        lda     #0
        sta     mag_x8 + 1
        sta     mag_y8 + 1
        ; Save 8 * mag_x
        txa

        .repeat 3
        asl     a
        rol     mag_x8 + 1
        .endrepeat

        sta     mag_x8
        ; Save 8 * mag_y
        tya

        .repeat 3
        asl     a
        rol     mag_y8 + 1
        .endrepeat

        sta     mag_y8
        ; Done!
        rts
.endproc

; ******************************************************************************

.proc OUTTEXT

        ; ----------------------------------------------------------------------
        ;
        ; OUTTEXT: Draw text at x1, y1. String is in ptr3
        ;
        ; ----------------------------------------------------------------------

; locals
        string  := tmp1
        cols    := tmp3
        pixels  := tmp4
        font    := regsave
.rodata
        ataint: .byte   64,0,32,96
.bss
        rows:   .res    1

.if >(::x_res - 1) > 0
        oldx1:  .res    2
        oldx2:  .res    2
.else
        oldx1:  .res    1
        oldx2:  .res    1
.endif

        oldy1:  .res    1
        oldy2:  .res    1
        inv:    .res    1

.code
        ; Don't draw zero sized characters
        lda     mag_x
        ora     mag_y
        bne     not0
        rts

not0:   ; Save string address, ptr3 is needed by BAR
        lda     ptr3
        sta     string
        lda     ptr3 + 1
        sta     string + 1

        bit     text_dir
        bmi     vert

        ; Calculate x2
        lda     mag_x
        sub     #1
        add     x1
        sta     x2

.if >(::x_res - 1) > 0
        lda     x1 + 1
        adc     #0
        sta     x2 + 1
.else
        lda     #0
        sta     x2 + 1
.endif

        ; Calculate y2 and adjust y1
        dec     y1
        lda     y1
        sta     y2
        sub     mag_y
        add     #1
        sta     y1
        lda     #0
        sta     y2 + 1

        jmp     while

        ; Calculate for vertical text
vert:   lda     x1
        sub     #1
        sta     x2
        lda     x1 + 1
        sbc     #0
        sta     x2 + 1
        lda     x1
        sub     mag_y
        sta     x1
        lda     x1 + 1
        sbc     #0
        sta     x1 + 1

        lda     mag_x
        sub     #1
        add     y1
        sta     y2
        lda     #0
        sta     y2 + 1
        jmp     while

        ; Main loop
loop:   inc     string
        bne     skiph
        inc     string + 1
skiph:  ; Save coords
        bit     text_dir
        bmi     scvert

        ldx     y1
        stx     oldy1
        ldx     y2
        stx     oldy2
        jmp     draw

scvert: ldx     x1
        stx     oldx1
        ldx     x2
        stx     oldx2

.if >(::x_res - 1) > 0
        ldx     x1 + 1
        stx     oldx1 + 1
        ldx     x2 + 1
        stx     oldx2 + 1
.endif

        ; Draw one character
        ; Convert to ANTIC code
draw:   tay
        rol     a
        rol     a
        rol     a
        rol     a
        and     #3
        tax
        tya
        and     #$9f
        ora     ataint,x
        ; Save and clear inverse video bit
        sta     inv
        and     #$7F
        ; Calculate font data address
        sta     font
        lda     #0
        sta     font + 1

        .repeat 3
        asl     font
        rol     a
        .endrepeat

        adc     CHBAS
        sta     font + 1
        ; Save old coords
        bit     text_dir
        bpl     hor
        lda     y1
        sta     oldy1
        lda     y2
        sta     oldy2
        jmp     cont
hor:    lda     x1
        sta     oldx1
        lda     x2
        sta     oldx2

.if >(::x_res - 1) > 0
        lda     x1 + 1
        sta     oldx1 + 1
        lda     x2 + 1
        sta     oldx2 + 1
.endif

        ; Get glyph pixels
cont:   ldy     #7
        ; Put one row of the glyph
putrow: sty     rows
        lda     (font),y
        bit     inv
        bpl     noinv
        eor     #$FF
noinv:  sta     pixels
        lda     #7
        sta     cols
        ; Put one column of the row
putcol: asl     pixels
        bcc     next_col
        lda     x1
        pha
        lda     x1 + 1
        pha
        jsr     clipped_bar
        pla
        sta     x1 + 1
        pla
        sta     x1
next_col:
        ; Go to next column
        ; increase x coords
        bit     text_dir
        bmi     vertinc

        lda     mag_x
        add     x1
        sta     x1
        bcc     L1
        inc     x1 + 1
L1:     lda     mag_x
        add     x2
        sta     x2
        bcc     L2
        inc     x2 + 1
        jmp     L2

vertinc:
        lda     y1
        sub     mag_x
        sta     y1
        lda     y2
        sub     mag_x
        sta     y2
L2:
        dec     cols
        bpl     putcol
next_row:
        ; Go to next row
        bit     text_dir
        bmi     verty

        lda     y1
        sub     mag_y
        sta     y1
        bcs     L3
        dec     y1 + 1
L3:     lda     y2
        sub     mag_y
        sta     y2
        bcs     L6
        dec     y2 + 1
L4:     jmp     L6

verty:  lda     x1
        sub     mag_y
        sta     x1
        bcs     L5
        dec     x1 + 1
L5:     lda     x2
        sub     mag_y
        sta     x2
        bcs     L6
        dec     x2 + 1
L6:
        ; Restore old values
        bit     text_dir
        bpl     reshor
        lda     oldy1
        sta     y1
        lda     oldy2
        sta     y2
        jmp     nextrow
reshor: lda     oldx1
        sta     x1
        lda     oldx2
        sta     x2

.if >(::x_res - 1) > 0
        lda     oldx1 + 1
        sta     x1 + 1
        lda     oldx2 + 1
        sta     x2 + 1
.endif

        ; Next row
nextrow:
        ldy     rows
        dey
        jpl     putrow

        ; Restore coords
        bit     text_dir
        bmi     resvert

        ldx     oldy1
        stx     y1
        ldx     oldy2
        stx     y2
        ldx     #0
        stx     y1 + 1
        stx     y2 + 1

        lda     mag_x8
        add     x1
        sta     x1
        lda     mag_x8 + 1
        adc     x1 + 1
        sta     x1 + 1
        lda     mag_x8
        add     x2
        sta     x2
        lda     mag_x8 + 1
        adc     x2 + 1
        sta     x2 + 1

        jmp     while

resvert:
        ldx     oldx1
        stx     x1
        ldx     oldx2
        stx     x2

.if >(::x_res - 1) > 0
        ldx     oldx1 + 1
        stx     x1 + 1
        ldx     oldx2 + 1
        stx     x2 + 1
.endif

        lda     y1
        sub     mag_x8
        sta     y1
        lda     y1 +1
        sbc     mag_x8 + 1
        sta     y1 + 1
        lda     y2
        sub     mag_x8
        sta     y2
        lda     y2 +1
        sbc     mag_x8 + 1
        sta     y2 + 1

        ; End of loop
while:  ldy     #0
        lda     (string),y
        jne     loop            ; Check for null character
        rts

.endproc

.if pages = 2
; ******************************************************************************

.proc SETVIEWPAGE

        ; ----------------------------------------------------------------------
        ;
        ; SETVIEWPAGE, page in A
        ;
        ; ----------------------------------------------------------------------

.code
        tax
        beq     cont
        lda     #32

cont:   add     p0dls
        cmp     SDLSTH
        beq     done            ; We're already in the desired page

        ldx     RTCLOK + 2
        sta     SDLSTH

        ; Wait until next VBLANK
wait:   cpx     RTCLOK + 2
        beq     wait

        ; Done
done:   rts
.endproc

; ******************************************************************************

.proc SETDRAWPAGE

        ; ----------------------------------------------------------------------
        ;
        ; SETDRAWPAGE, page in A
        ;
        ; ----------------------------------------------------------------------

.code
        tax
        beq     cont
        lda     #32
cont:   add     p0scr
        sta     SAVMSC + 1
        rts
.endproc
.endif

; ******************************************************************************

        ; ----------------------------------------------------------------------
        ;
        ; Unimplemented functions that require an error code
        ;
        ; ----------------------------------------------------------------------

CONTROL:
        lda     #TGI_ERR_INV_FUNC
        sta     error
        ; fall through

; ******************************************************************************

        ; ----------------------------------------------------------------------
        ;
        ; Unimplemented functions that don't require an error code
        ;
        ; ----------------------------------------------------------------------

INSTALL:
UNINSTALL:

.if pages = 1
SETVIEWPAGE:
SETDRAWPAGE:
.endif

        rts
