;
; cputc/cputcxy for Challenger 1P
; Based on PET/CBM implementation
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;
        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot
        .import         popa, _gotoxy

        .include        "osic1p.inc"
        .include        "extzp.inc"

_cputcxy:
        pha                     ; Save C
        jsr     popa            ; Get Y
        jsr     _gotoxy         ; Set cursor, drop x
        pla                     ; Restore C

; Plot a character - also used as internal function

_cputc: cmp     #$0A            ; CR?
        bne     L1
        lda     #0
        sta     CURS_X
        beq     plot            ; Recalculate pointers

L1:     cmp     #$0D            ; LF?
        beq     newline         ; Recalculate pointers

cputdirect:
        jsr     putchar         ; Write the character to the screen

; Advance cursor position

advance:
        cpy     #SCR_LINELEN    ; xsize-1
        bne     L3
        jsr     newline         ; new line
        ldy     #$FF            ; + cr
L3:     iny
        sty     CURS_X
        rts

newline:
        inc     CURS_Y
        lda     CURS_Y
        cmp     #SCR_HEIGHT     ; screen height
        bne     plot
        lda     #0              ; wrap around to line 0
        sta     CURS_Y

plot:   ldy     CURS_Y
        lda     ScrLo,y
        sta     SCREEN_PTR
        lda     ScrHi,y
        sta     SCREEN_PTR+1
        rts

; Write one character to the screen without doing anything else, return X
; position in Y

putchar:
        ldy     CURS_X
        sta     (SCREEN_PTR),y  ; Set char
        rts

; Screen address tables - offset to real screen

.rodata

ScrLo:  .byte   $85, $A5, $C5, $E5, $05, $25, $45, $65
        .byte   $85, $A5, $C5, $E5, $05, $25, $45, $65
        .byte   $85, $A5, $C5, $E5, $05, $25, $45, $65
        .byte   $85

ScrHi:  .byte   $D0, $D0, $D0, $D0, $D1, $D1, $D1, $D1
        .byte   $D1, $D1, $D1, $D1, $D2, $D2, $D2, $D2
        .byte   $D2, $D2, $D2, $D2, $D3, $D3, $D3, $D3
        .byte   $D3
