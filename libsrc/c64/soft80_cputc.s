;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         soft80_cputcxy, soft80_cputc
        .export         soft80_cputdirect, soft80_putchar
        .export         putcolor        ; FIX/CHECK

        .export         soft80_newline, soft80_plot
        .import         popa, _gotoxy
        .import         xsize
        .import         PLOT            ; FIX/CHECK
        .importzp       tmp4,tmp3
        .import         __bgcolor               ; FIX/CHECK

        .macpack        longbranch

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
        jmp     PLOT            ; Set the new cursor

L1:     cmp     #$0D            ; LF?
        beq     soft80_newline         ; Recalculate pointers

        ; Printable char of some sort

        tay
        bpl     L10

        clc
        adc     #$20
        and     #$7F
L10:

soft80_cputdirect:
        jsr     soft80_putchar         ; Write the character to the screen

; Advance cursor position

advance:
        iny
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

;--- start color vodoo

; remove color from cell
; y unmodified
remcolor:

        ;ldy     #$00            ; is still $00

        lda     (CRAM_PTR),y    ; vram
        and     #$0f
        cmp     __bgcolor
        jeq     @l2b            ; vram==bgcolor

        inc     $01
        lda     (CRAM_PTR),y    ; colram
        stx     $01             ;$34
        and     #$0f
        cmp     __bgcolor
        beq     @l2s            ; colram==bgcolor

        ; vram = colram
        ;inc     $01
        ;lda     (CRAM_PTR),y    ; colram
        ;stx     $01             ;$34
        ;and     #$0f

        sta     tmp3
        lda     (CRAM_PTR),y    ; vram
        and     #$f0
        ora     tmp3
        sta     (CRAM_PTR),y    ; vram

        ; colram = bgcolor
        lda     __bgcolor
        inc     $01
        sta     (CRAM_PTR),y    ; colram
        stx     $01 ;$34

        jmp     @l2b

@l2s:
        ; colram is bgcolor
        ; => only one char in cell used

        jsr     soft80_checkchar
        bcc     @l2b            ; space at current position

        ; vram = bgcolor
        lda     (CRAM_PTR),y    ; vram
        and     #$f0
        ora     __bgcolor
        sta     (CRAM_PTR),y    ; vram
@l2b:
        rts

; put color to cell
; y unmodified
putcolor:

        ;ldy     #$00            ; is still $00

        lda     (CRAM_PTR),y    ; vram
        and     #$0f
        cmp     __bgcolor
        beq     @l2s            ; vram==bgcolor => first char in cell

        ; vram!=bgcolor => second char in cell

        inc     $01             ;$35
        lda     (CRAM_PTR),y    ; colram
        stx     $01             ;$34
        and     #$0f
        cmp     __bgcolor
        bne     @l2s            ; colram!=bgcolor

        ; colram==bgcolor => second char in cell or overwrite 1st char

        jsr     soft80_checkchar
        bcs     @l2a            ; char at current position => overwrite 1st

        ; colram=vram
        lda     (CRAM_PTR),y    ; vram
        inc     $01
        sta     (CRAM_PTR),y    ; colram
        stx     $01 ;$34

        ;jmp     @l2a

@l2s:
        ; colram!=bgcolor => alread 2 chars in cell
@l2a:

        ; Set color
        lda     CHARCOLOR
        sta     (CRAM_PTR),y    ; vram

        rts


;--- end color vodoo

        .export soft80_checkchar

; test if there is a space or a character at current position
; CLC: space        SEC: character
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

; output space

_space:

        lda     RVS
        jne     _spaceinvers

        jsr     remcolor

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

_spaceinvers:

        jsr     putcolor

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

; output a character

soft80_putchar:
        sta     tmp3

        sei
        ldx     $01
        stx     tmp4
        ldx     #$34

        stx     $01             ; will stay $34 for space
        ldy     #$00            ; will be $00 from now on

        cmp     #' '            ; space is a special (optimized) case
        jeq     _space

        jsr     putcolor

; output character
char:
        ldx     tmp3

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
