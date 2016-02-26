;
; Groepaz/Hitmen, 11.10.2015
;
; high level implementation for the soft80 implementation
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         soft80_cputcxy, soft80_cputc
        .export         soft80_cputdirect, soft80_putchar
        .export         soft80_newline, soft80_plot
        .export         soft80_checkchar

        .import         popa, _gotoxy

        .import         soft80_kplot
        .import         soft80_internal_bgcolor, soft80_internal_cellcolor
        .import         soft80_internal_cursorxlsb

        .importzp       tmp4,tmp3

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_cputcxy:
        pha                     ; Save C
        jsr     popa            ; Get Y
        jsr     _gotoxy         ; Set cursor, drop x
        pla                     ; Restore C

; Plot a character - also used as internal function

soft80_cputc:
        cmp     #$0A            ; CR?
        bne     L1

        lda     #0
        sta     CURS_X

        ; Set cursor position, calculate RAM pointers
soft80_plot:
        ldx     CURS_Y
        ldy     CURS_X
        clc
        jmp     soft80_kplot    ; Set the new cursor

L1:     cmp     #$0D            ; LF?
        beq     soft80_newline  ; Recalculate pointers

        ; shortcut for codes < $80 ... codes $20-$7f can be printed directly,
        ; codes $00-$1f are control codes which are not printable and thus may
        ; give undefined result.
        tay
        bpl     @L10

        ; codes $80-$ff must get converted like this:
        ; $80-$9f  ->   dont care (control codes)
        ; $a0-$bf  ->   $00-$1f
        ; $c0-$df  ->   $60-$7f
        ; $e0-$ff  ->   $00-$1f

        ora     #%01000000      ; $40
        clc
        adc     #%00100000      ; $20
        and     #%01111111      ; $7f
@L10:

        ; entry point for direct output of a character. the value passed in
        ; akku must match the offset in the charset.
        ; - the following may not modify tmp1
soft80_cputdirect:
        jsr     soft80_putchar  ; Write the character to the screen

        ; Advance cursor position
        iny                     ; contains CURS_X
        cpy     #charsperline
        beq     @L3

        sty     CURS_X
        tya
        and     #$01
        sta     soft80_internal_cursorxlsb
        bne     @L5

        lda     SCREEN_PTR
        clc
        adc     #8
        sta     SCREEN_PTR
        bcc     @L4
        inc     SCREEN_PTR+1
@L4:
        inc     CRAM_PTR
        bne     @L5
        inc     CRAM_PTR+1
@L5:
        rts
@L3:
        inc     CURS_Y          ; new line
        ldy     #0              ; + cr
        sty     CURS_X
        jmp     soft80_plot

        ; - the following may not modify tmp1
soft80_newline:

        lda     SCREEN_PTR
        clc
        adc     #<(40*8)
        sta     SCREEN_PTR

        lda     SCREEN_PTR+1
        adc     #>(40*8)
        sta     SCREEN_PTR+1

        lda     CRAM_PTR
        clc
        adc     #40
        sta     CRAM_PTR
        bcc     @L5
        inc     CRAM_PTR+1
@L5:
        inc     CURS_Y
        rts

;-------------------------------------------------------------------------------
; All following code belongs to the character output to bitmap
;
; this stuff is going to be used a lot so we unroll it a bit for speed
;-------------------------------------------------------------------------------

.if SOFT80FASTSPACE = 1

; output inverted space (odd)
draw_spaceinvers_odd:
        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$f0
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat
        jmp     draw_back

; output inverted space (general entry point)
; in: y must be $00
draw_spaceinvers:

.if SOFT80COLORVOODOO = 1
        jsr     soft80_putcolor
.else
        lda     soft80_internal_cellcolor
        sta     (CRAM_PTR),y    ; vram
.endif

        lda     soft80_internal_cursorxlsb
        bne     draw_spaceinvers_odd

; output inverted space (even)
        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$0f
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat
        jmp     draw_back

; output space (odd)
draw_space_odd:
        .repeat 8,line
        lda     (SCREEN_PTR),y
        ora     #$0f
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat
        jmp     draw_back

; output space (general entry point)
; in: y must be $00
draw_space:

        lda     RVS
        bne     draw_spaceinvers

.if SOFT80COLORVOODOO = 1
        jsr     remcolor
.endif
        ;ldy     #$00            ; is still $00

        lda     soft80_internal_cursorxlsb
        bne     draw_space_odd

; output space (even)
        .repeat 8,line
        lda     (SCREEN_PTR),y
        ora     #$f0
        sta     (SCREEN_PTR),y
        .if (line < 7)
        iny
        .endif
        .endrepeat
        jmp     draw_back
.endif

;-------------------------------------------------------------------------------
; output one character in internal encoding without advancing cursor position
; generic entry point
;
; - the following may not modify tmp1
; in:   A: charcode
; out:  Y: CURS_X
;
soft80_putchar:
        sta     tmp3            ; remember charcode

        sei
        ldx     $01
        stx     tmp4
        ldx     #$34

        stx     $01             ; will stay $34 for space
        ldy     #$00            ; will be $00 from now on

.if SOFT80FASTSPACE = 1
        cmp     #' '            ; space is a special (optimized) case
        beq     draw_space
.endif

.if SOFT80COLORVOODOO = 1
        jsr     soft80_putcolor
.else
        lda     soft80_internal_cellcolor
        sta     (CRAM_PTR),y    ; vram
.endif

; output character
        ldx     tmp3            ; get charcode

        lda     RVS
        beq     @skp
        jmp     draw_charinvers
@skp:
        lda     soft80_internal_cursorxlsb
        bne     draw_char_even

; output character (odd)
        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$0f
        ora     soft80_hi_charset+(line*$80),x
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat
        jmp     draw_back

; output character (even)
draw_char_even:
        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$f0
        ora     soft80_lo_charset+(line*$80),x
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat

draw_back:
        lda     tmp4
        sta     $01
        cli

        ldy     CURS_X
        rts

; output inverted character (odd)
draw_charinvers_odd:
        .repeat 8,line
        lda     (SCREEN_PTR),y
        ora     #$0f
        eor     soft80_lo_charset+(line*$80),x
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat
        jmp     draw_back

; output inverted character (generic)
draw_charinvers:
        lda     soft80_internal_cursorxlsb
        bne     draw_charinvers_odd

        .repeat 8,line
        lda     (SCREEN_PTR),y
        ora     #$f0
        eor     soft80_hi_charset+(line*$80),x
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat
        jmp     draw_back

;-------------------------------------------------------------------------------
; optional "color voodoo". the problem is that each 8x8 cell can only contain
; two colors, one of which is used for the background color, so two characters
; have to share the same text color.
;
; - in a cell that contains two spaces, both the color ram and the text color
;   in vram contain the background color
;
; - in a cell that contains one character, its text color goes into vram. the
;   color ram contains the background color.
;
; - in a cell that contains two characters, the color of the left character goes
;   to vram (and is shared by both for display). the "would be" color of the
;   right character goes to color ram as a reminder and can be restored when one
;   of the two characters is cleared by a space.

.if SOFT80COLORVOODOO = 1

; remove color from cell, called before putting a "space" character to the bitmap
;
; __ -> __      -
; _A -> _A      -
; B_ -> B_      -
; _A -> __      vram = bgcol
; B_ -> __      vram = bgcol
; BA -> _A      vram = colram, colram = bgcol
; BA -> B_      colram = bgcol
;
; in:  x must be $34
;      y must be $00
; out: x = $34
;      y = $00
remcolor:

        ;ldy     #$00            ; is still $00

        ; if the textcolor in vram is equal to the background color, then
        ; no (visible) character is in the current cell and we can exit
        ; immediately.
        lda     (CRAM_PTR),y    ; vram (textcolor)
        and     #$0f
        cmp     soft80_internal_bgcolor
        beq     @sk1            ; yes, vram==bgcolor

        ; now check if the textcolor in color ram is equal the background color,
        ; if yes then there is only one (visible) character in the current cell
        inc     $01             ; $35
        lda     (CRAM_PTR),y    ; colram (2nd textcolor)
        stx     $01             ; $34
        and     #$0f
        cmp     soft80_internal_bgcolor
        beq     @sk2            ; yes, colram==bgcolor
        sta     tmp3            ; A contains colram

        ; two characters in the current cell, of which one will get removed

        lda     soft80_internal_cursorxlsb
        bne     @sk3

        ; vram = colram
        lda     (CRAM_PTR),y    ; vram
        and     #$f0
        ora     tmp3            ; colram value
        sta     (CRAM_PTR),y    ; vram
@sk3:
        ; colram = bgcolor
        lda     soft80_internal_bgcolor
        inc     $01             ; $35
        sta     (CRAM_PTR),y    ; colram
        stx     $01             ; $34

        rts

@sk2:
        ; colram is bgcolor
        ; => only one char in cell used

        jsr     soft80_checkchar
        bcs     @sk1            ; space at current position

        ; vram (textcolor) = bgcolor
        lda     (CRAM_PTR),y    ; vram
        and     #$f0
        ora     soft80_internal_bgcolor
        sta     (CRAM_PTR),y    ; vram
@sk1:
        rts

; put color to cell
;
; __ -> _A      vram = textcol
; __ -> B_      vram = textcol
; _A -> BA      colram = vram, vram = textcol
; B_ -> BA      colram = textcol
;
; _A -> _C      vram = textcol
; B_ -> C_      vram = textcol
; BA -> BC      colram = textcol
; BA -> CA      vram = textcol
;
; in:  $01 is $34 (RAM under I/O) when entering
;      x must be $34
;      y must be $00
; out: x = $34
;      y = $00
soft80_putcolor:

        ;ldy     #$00            ; is still $00

        lda     (CRAM_PTR),y    ; vram
        and     #$0f
        cmp     soft80_internal_bgcolor
        beq     @sk1            ; vram==bgcolor => first char in cell

        ; vram!=bgcolor => second char in cell

        inc     $01             ; $35
        lda     (CRAM_PTR),y    ; colram
        stx     $01             ; $34
        and     #$0f
        cmp     soft80_internal_bgcolor
        beq     @l2s            ; colram==bgcolor -> second char in cell

        ; botch characters in the cell are used

        lda     soft80_internal_cursorxlsb
        bne     @sk2            ; jump if odd xpos

        ; vram = textcol
        lda     soft80_internal_cellcolor
        sta     (CRAM_PTR),y    ; vram
        rts

@l2s:
        ; one character in cell is already used
        jsr     soft80_checkchar
        bcc     @sk1            ; char at current position => overwrite 1st

        lda     soft80_internal_cursorxlsb
        beq     @sk3            ; jump if even xpos
@sk2:
        ; colram = textcol
        lda     CHARCOLOR
        inc     $01             ; $35
        sta     (CRAM_PTR),y    ; colram
        stx     $01             ; $34
        rts

@sk3:
        ; colram=vram
        lda     (CRAM_PTR),y    ; vram
        inc     $01             ; $35
        sta     (CRAM_PTR),y    ; colram
        stx     $01             ; $34
@sk1:
        ; vram = textcol
        lda     soft80_internal_cellcolor
        sta     (CRAM_PTR),y    ; vram
        rts

;
; test if there is a space or a character at current position
;
; in:  x = $34
;      $01 must be $34
;
; out: SEC: space
;      CLC: character
;      x = $34
;      y = $00
soft80_checkchar:

        lda     soft80_internal_cursorxlsb
        bne     @l1a

        ; check charset data from bottom up, since a lot of eg lowercase chars
        ; have no data in the top rows, but all of them DO have data in the
        ; second to bottom row, this will likely be faster in average.

        ldy     #7
        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$f0
        cmp     #$f0
        bne     @ischar
        .if (line < 7)
        dey
        .endif
        .endrepeat
        ;ldy     #$00                            ; is 0
        ;sec                                     ; is set
        rts
@ischar:
        ldy     #$00
        ;clc                                     ; is cleared
        rts
@l1a:
        ldy     #$07
        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$0f
        cmp     #$0f
        bne     @ischar
        .if line < 7
        dey
        .endif
        .endrepeat
        ;ldy     #$00                            ; is 0
        ;sec                                     ; is set
        rts
.endif
