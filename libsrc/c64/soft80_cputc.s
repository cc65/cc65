;
; Groepaz/Hitmen, 11.10.2015
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         soft80_cputcxy, soft80_cputc
        .export         soft80_cputdirect, soft80_putchar
        .export         soft80_newline, soft80_plot

        .import         popa, _gotoxy
        .import         xsize
        .import         soft80_kplot
        .import         __bgcolor, __textcolor

        .importzp       tmp4,tmp3

        .macpack        longbranch

        .include        "c64.inc"
        .include        "soft80.inc"

.if SOFT80COLORVOODOO = 1
        .export         soft80_putcolor
.endif

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

        ; Printable char of some sort
        tay
        bpl     L10

        ; extra check for petscii codes 160-191, these have been moved to
        ; 0-31 in the charset
        and     #%11100000
        cmp     #%10100000
        bne     @sk

        tya
        and     #%00011111
        bpl     L10             ; branch always
@sk:
        tya
        clc
        adc     #$20
        and     #$7F
L10:

soft80_cputdirect:
        jsr     soft80_putchar  ; Write the character to the screen

; Advance cursor position

advance:
        iny                     ; contains CURS_X
        cpy     #charsperline
        beq     L3

        sty     CURS_X
        tya
        and     #$01
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
L3:
        inc     CURS_Y          ; new line
        ldy     #0              ; + cr
        sty     CURS_X
        jmp     soft80_plot

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
        bcc     L5
        inc     CRAM_PTR+1
L5:
        inc     CURS_Y
        rts

; Write one character to the screen without doing anything else
; in:         A:  character
; returns:    Y:  cursor X position
; this function is going to be used a lot so we unroll it a bit for speed

.if SOFT80FASTSPACE = 1
; output space
; in: y must be $00
_space:

        lda     RVS
        jne     _spaceinvers

.if SOFT80COLORVOODOO = 1
        jsr     remcolor
.endif
        ;ldy     #$00            ; is still $00

        lda     CURS_X
        and     #$01
        bne     @l1

        .repeat 8,line
        lda     (SCREEN_PTR),y
        ora     #$f0
        sta     (SCREEN_PTR),y
        .if (line < 7)
        iny
        .endif
        .endrepeat
        jmp     _back
@l1:
        .repeat 8,line
        lda     (SCREEN_PTR),y
        ora     #$0f
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat
@l2:
        jmp     _back

; output inverted space
; in: y must be $00
_spaceinvers:

.if SOFT80COLORVOODOO = 1
        jsr     soft80_putcolor
.else
        lda     CHARCOLOR
        sta     (CRAM_PTR),y    ; vram
.endif

        lda     CURS_X
        and     #$01
        bne     @l1

        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$0f
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat
        jmp     _back
@l1:
        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$f0
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat

        jmp     _back
.endif

; output a character

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
        jeq     _space
.endif

.if SOFT80COLORVOODOO = 1
        jsr     soft80_putcolor
.else
        lda     CHARCOLOR
        sta     (CRAM_PTR),y    ; vram
.endif
        ; output character

        ldx     tmp3            ; get charcode

        lda     RVS
        jne     _invers

        lda     CURS_X
        and     #$01
        bne     @l1

        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$0f
        ora     soft80_hi_charset+(line*$80),x
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat
        jmp     @l2
@l1:

        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$f0
        ora     soft80_lo_charset+(line*$80),x
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat

@l2:

_back:
        lda     tmp4
        sta     $01
        cli

        ldy     CURS_X
        rts

; output inverted character
_invers:

        lda     CURS_X
        and     #$01
        bne     @l1

        .repeat 8,line
        lda     (SCREEN_PTR),y
        ora     #$f0
        eor     soft80_hi_charset+(line*$80),x
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat
        jmp     _back
@l1:
        .repeat 8,line
        lda     (SCREEN_PTR),y
        ora     #$0f
        eor     soft80_lo_charset+(line*$80),x
        sta     (SCREEN_PTR),y
        .if line < 7
        iny
        .endif
        .endrepeat
        jmp     _back

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
; out: y = $00
remcolor:

        ;ldy     #$00            ; is still $00

        ; if the textcolor in vram is equal to the background color, then
        ; no (visible) character is in the current cell and we can exit
        ; immediately.
        lda     (CRAM_PTR),y    ; vram (textcolor)
        and     #$0f
        cmp     __bgcolor
        beq     @sk1            ; yes, vram==bgcolor

        ; now check if the textcolor in color ram is equal the background color,
        ; if yes then there is only one (visible) character in the current cell
        inc     $01             ; $35
        lda     (CRAM_PTR),y    ; colram (2nd textcolor)
        stx     $01             ; $34
        and     #$0f
        cmp     __bgcolor
        beq     @sk2            ; yes, colram==bgcolor

        ; two characters in the current cell, of which one will get removed

        ; vram = colram
        ;inc     $01
        ;lda     (CRAM_PTR),y    ; colram
        ;stx     $01             ;$34
        ;and     #$0f
        sta     tmp3            ; A contains colram

        lda     CURS_X
        and     #$01
        bne     @sk3

        ; vram = colram
        lda     (CRAM_PTR),y    ; vram
        and     #$f0
        ora     tmp3
        sta     (CRAM_PTR),y    ; vram
@sk3:
        ; colram = bgcolor
        lda     __bgcolor
        inc     $01             ; $35
        sta     (CRAM_PTR),y    ; colram
        stx     $01             ; $34

        rts

@sk2:
        ; colram is bgcolor
        ; => only one char in cell used

        jsr     soft80_checkchar
        bcc     @sk1            ; space at current position

        ; vram (textcolor) = bgcolor
        lda     (CRAM_PTR),y    ; vram
        and     #$f0
        ora     __bgcolor
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
        cmp     __bgcolor
        beq     @sk1            ; vram==bgcolor => first char in cell

        ; vram!=bgcolor => second char in cell

        inc     $01             ; $35
        lda     (CRAM_PTR),y    ; colram
        stx     $01             ; $34
        and     #$0f
        cmp     __bgcolor
        beq     @l2s            ; colram==bgcolor -> second char in cell

        ; botch characters in the cell are used

        lda     CURS_X
        and     #$01
        bne     @sk2            ; jump if odd xpos

        ; vram = textcol
        lda     CHARCOLOR
        sta     (CRAM_PTR),y    ; vram
        rts

@l2s:
        ; one character in cell is already used
        jsr     soft80_checkchar
        bcs     @sk1            ; char at current position => overwrite 1st

        lda     CURS_X
        and     #$01
        beq     @sk3            ; jump if even xpos
@sk2:
        ; colram = textcol
        lda     __textcolor
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
        lda     CHARCOLOR
        sta     (CRAM_PTR),y    ; vram
        rts

; test if there is a space or a character at current position
; in:  y must be $00
; out: CLC: space        SEC: character
;      y = $00
soft80_checkchar:

        ;ldy     #$00            ; is still $00

        lda     CURS_X
        and     #$01
        jne     @l1a

        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$f0
        cmp     #$f0
        bne     @l2b
        .if (line < 7)
        iny
        .endif
        .endrepeat

        ldy     #$00
        clc
        rts
@l2b:
        ldy     #$00
        sec
        rts
@l1a:
        .repeat 8,line
        lda     (SCREEN_PTR),y
        and     #$0f
        cmp     #$0f
        bne     @l2bb
        .if line < 7
        iny
        .endif
        .endrepeat
        ldy     #$00
        clc
        rts
@l2bb:
        ldy     #$00
        sec
        rts

.endif
