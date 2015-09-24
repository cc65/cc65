;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot
        .import         popa, _gotoxy
        .import         PLOT
        .import         xsize

        .importzp       tmp3,tmp4

        .include        "pce.inc"
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

        ora     RVS             ; Set revers bit

        tax

        st0     #VDC_MAWR       ; Memory Adress Write

        lda     SCREEN_PTR
        sta     a:VDC_DATA_LO

        lda     SCREEN_PTR + 1
        sta     a:VDC_DATA_HI

        st0     #VDC_VWR        ; VWR

        txa
        sta     a:VDC_DATA_LO   ; character

        lda     CHARCOLOR

        asl     a
        asl     a
        asl     a
        asl     a

        ora     #$02
        sta     a:VDC_DATA_HI

        rts

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import initconio
conio_init      = initconio
