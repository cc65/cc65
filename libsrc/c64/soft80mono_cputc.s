;
; Groepaz/Hitmen, 19.10.2015
;
; high level implementation for the monochrome soft80 implementation
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         soft80mono_cputcxy, soft80mono_cputc
        .export         soft80mono_cputdirect, soft80mono_putchar
        .export         soft80mono_newline, soft80mono_plot

        .import         popa, _gotoxy

        .import         soft80mono_kplot
        .import         soft80mono_internal_bgcolor, soft80mono_internal_cellcolor
        .import         soft80mono_internal_cursorxlsb, soft80mono_internal_nibble

        .importzp       tmp4, tmp3, ptr2

        .include        "c64.inc"
        .include        "soft80.inc"

soft80mono_cputcxy:
        pha                     ; Save C
        jsr     popa            ; Get Y
        jsr     _gotoxy         ; Set cursor, drop x
        pla                     ; Restore C

; Plot a character - also used as internal function

soft80mono_cputc:
        cmp     #$0A            ; CR?
        bne     L1

        lda     #0
        sta     CURS_X

        ; Set cursor position, calculate RAM pointers
soft80mono_plot:
        ldx     CURS_Y
        ldy     CURS_X
        clc
        jmp     soft80mono_kplot        ; Set the new cursor

L1:     cmp     #$0D                    ; LF?
        beq     soft80mono_newline      ; Recalculate pointers

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
soft80mono_cputdirect:
        jsr     soft80mono_putchar      ; Write the character to the screen

        ; Advance cursor position
        iny                             ; contains CURS_X
        cpy     #charsperline
        beq     @L3

        sty     CURS_X
        tya
        and     #$01
        sta     soft80mono_internal_cursorxlsb
        bne     @L4

        lda     SCREEN_PTR
        clc
        adc     #8
        sta     SCREEN_PTR
        bcc     @L4
        inc     SCREEN_PTR+1
@L4:
        rts
@L3:
        inc     CURS_Y          ; new line
        ldy     #0              ; + cr
        sty     CURS_X
        jmp     soft80mono_plot

        ; - the following may not modify tmp1
soft80mono_newline:

        lda     SCREEN_PTR
        clc
        adc     #<(40*8)
        sta     SCREEN_PTR

        lda     SCREEN_PTR+1
        adc     #>(40*8)
        sta     SCREEN_PTR+1

        inc     CURS_Y
        rts

;-------------------------------------------------------------------------------
; output one character in internal encoding without advancing cursor position
; generic entry point
;
; - the following may not modify tmp1
; in:   A: charcode
; out:  Y: CURS_X
;
soft80mono_putchar:
        sta     tmp3            ; save charcode

        sei
        lda     $01
        pha
        lda     #$34
        sta     $01             ; enable RAM under I/O

        ldy     #$00            ; will be $00 from now on

        ldx     soft80mono_internal_cursorxlsb
        lda     chardatal,x
        clc
        adc     tmp3
        sta     ptr2
        lda     chardatah,x
        adc     #0
        sta     ptr2+1

        lda     RVS
        bne     draw_charinvers

        lda     nibble,x
        sta     tmp3

        ;ldy     #0                      ; is still $00
@lp1:
        lda     (SCREEN_PTR),y
        and     tmp3
        ora     (ptr2),y
        sta     (SCREEN_PTR),y
        clc
        lda     ptr2
        adc     #$7f
        sta     ptr2
        bcc     @sk1
        inc     ptr2+1
@sk1:
        iny
        cpy     #8
        bne     @lp1

draw_back:
        pla
        sta     $01
        cli

        ldy     CURS_X
        rts

; output inverted character
draw_charinvers:
        lda     soft80mono_internal_nibble,x
        sta     tmp3

        ;ldy     #0                      ; is still $00
@lp1:
        lda     (SCREEN_PTR),y
        ora     tmp3
        eor     (ptr2),y
        sta     (SCREEN_PTR),y
        clc
        lda     ptr2
        adc     #$7f
        sta     ptr2
        bcc     @sk1
        inc     ptr2+1
@sk1:
        iny
        cpy     #8
        bne     @lp1
        jmp     draw_back

        .rodata
chardatal:
        .byte <soft80_hi_charset
        .byte <soft80_lo_charset
chardatah:
        .byte >soft80_hi_charset
        .byte >soft80_lo_charset
nibble:
        .byte $0f, $f0

