;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot
        .forceimport    initconio       ; force conio initiation

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
        stz     CURS_X
        bra     plot            ; Recalculate pointer

L1:     cmp     #$0A            ; LF?
        beq     newline         ; Recalculate pointer

; Printable char of some sort

cputdirect:
        jsr     putchar         ; Write the character to the screen

; Move the cursor (rightwards) to the next position.

advance:
        ldy     CURS_X
        iny
        cpy     xsize
        bne     L3
        inc     CURS_Y          ; new line
        cly                     ; + CR
L3:     sty     CURS_X

; Set cursor position; calculate VRAM pointer.

plot:   ldy     CURS_X
        ldx     CURS_Y
        clc
        jmp     PLOT            ; Set the new cursor

newline:
        inc     CURS_Y
        bra     plot

; Write one character to the screen without doing anything else.

putchar:
        ora     RVS             ; Set reverse bit

        st0     #VDC_MAWR       ; Memory-Address Write
        ldy     SCREEN_PTR
        ldx     SCREEN_PTR+1
        sty     VDC_DATA_LO
        stx     VDC_DATA_HI

        st0     #VDC_VWR
        sta     VDC_DATA_LO     ; character

        lda     CHARCOLOR       ; pallette number
        asl     a
        asl     a
        asl     a
        asl     a
        ora     #>$0200         ; high nybble of char. index
        sta     VDC_DATA_HI

        rts
