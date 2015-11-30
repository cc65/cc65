;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot
        .import         popa, _gotoxy
        .import         PLOT
        .import         xsize
        .import         fontdata
        .import         _plotlo

        .importzp       tmp3,tmp4
        .importzp       ptr3

        .include        "gamate.inc"
        .include        "extzp.inc"

_cputcxy:
        pha                     ; Save C
        jsr     popa            ; Get Y
        jsr     _gotoxy         ; Set cursor, drop x
        pla                     ; Restore C

; Plot a character - also used as internal function

_cputc: cmp     #$0d            ; CR?
        bne     L1
        lda     #0
        sta     CURS_X
        beq     plot            ; Recalculate pointers

L1:     cmp     #$0a            ; LF?
        beq     newline         ; Recalculate pointers

; Printable char of some sort

cputdirect:
        jsr     putchar         ; Write the character to the screen

; Advance cursor position

advance:
        ldy     CURS_X
        iny
        cpy     xsize
        bne     L3
        jsr     newline         ; new line
        ldy     #0              ; + cr
L3:     sty     CURS_X
        jmp     plot

newline:
        inc     CURS_Y

; Set cursor position, calculate RAM pointers

plot:   ldy     CURS_X
        ldx     CURS_Y
        clc
        jmp     PLOT            ; Set the new cursor

; Write one character to the screen without doing anything else, return X
; position in Y

putchar:
        sta     ptr3

        txa
        pha

        lda     #0
        sta     ptr3+1

        ; char index * 8
        asl     ptr3
        rol     ptr3+1
        asl     ptr3
        rol     ptr3+1
        asl     ptr3
        rol     ptr3+1

        ; plus fontdata base address
        lda     ptr3
        clc
        adc     #<(fontdata-$f8)
        sta     ptr3
        lda     ptr3+1
        adc     #>(fontdata-$f8)
        sta     ptr3+1

        lda     CHARCOLOR
        and     #1
        beq     @skip_plane1

        lda     #LCD_XPOS_PLANE1
        clc
        adc     CURS_X
        sta     LCD_X

        ldy     #$f8
@copylp1:
        lda     (ptr3),y
        eor     RVS
        sta     LCD_DATA
        iny
        bne     @copylp1

@skip_plane1:

        lda     CHARCOLOR
        and     #2
        beq     @skip_plane2

        lda     #LCD_XPOS_PLANE2
        clc
        adc     CURS_X
        sta     LCD_X

        ldx     CURS_Y
        lda     _plotlo,x
        sta     LCD_Y

        ldy     #$f8
@copylp2:
        lda     (ptr3),y
        eor     RVS
        sta     LCD_DATA
        iny
        bne     @copylp2

@skip_plane2:
        pla
        tax
        ldy     CURS_X
        rts

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import initconio
conio_init      = initconio
