;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot

        .import         gotoxy
        .import         PLOT
        .import         xsize
        .importzp       tmp3, tmp4

        .include        "pce.inc"
        .include        "extzp.inc"

_cputcxy:
        pha                     ; Save C
        jsr     gotoxy          ; Set cursor, drop x and y
        pla                     ; Restore C

; Plot a character - also used as internal function

_cputc: cmp     #$0D            ; CR?
        bne     L1
        lda     #0
        sta     CURS_X
        beq     plot            ; Recalculate pointer

L1:     cmp     #$0A            ; LF?
        beq     newline         ; Recalculate pointer

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
        ldy     #0              ; + CR
L3:     sty     CURS_X
        jmp     plot

newline:
        inc     CURS_Y

; Set cursor position; calculate VRAM pointer.

plot:   ldy     CURS_X
        ldx     CURS_Y
        clc
        jmp     PLOT            ; Set the new cursor

; Write one character to the screen without doing anything else.

putchar:
        ora     RVS             ; Set revers bit

        tax

        st0     #VDC_MAWR       ; Memory Address Write

        lda     SCREEN_PTR
        sta     VDC_DATA_LO

        lda     SCREEN_PTR+1
        sta     VDC_DATA_HI

        st0     #VDC_VWR        ; VWR

        txa
        sta     VDC_DATA_LO     ; character

        lda     CHARCOLOR       ; pallette number
        asl     a
        asl     a
        asl     a
        asl     a

        ora     #$02
        sta     VDC_DATA_HI

        rts

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import initconio
conio_init      = initconio
