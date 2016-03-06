;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline
        .constructor    initconio
        .import         popa, _gotoxy
        .import         ppuinit, paletteinit, ppubuf_put
        .import         setcursor

        .importzp       tmp3,tmp4

        .include        "nes.inc"

;-----------------------------------------------------------------------------

.code

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
        cpy     #xsize
        bne     L3
        inc     CURS_Y          ; new line
        ldy     #0              ; + cr
L3:     sty     CURS_X
        jmp     plot

newline:
        inc     CURS_Y

; Set cursor position, calculate RAM pointers

plot:   ldy     CURS_X
        ldx     CURS_Y
        jmp     setcursor       ; Set the new cursor


; Write one character to the screen without doing anything else, return X
; position in Y

putchar:
        ora     RVS             ; Set revers bit
        ldy     SCREEN_PTR+1
        ldx     SCREEN_PTR
        jmp     ppubuf_put

;-----------------------------------------------------------------------------
; Initialize the conio subsystem. Code goes into the ONCE segment, which may
; be reused after startup.

.segment        "ONCE"

initconio:
        jsr     ppuinit
        jsr     paletteinit

        lda     #0
        sta     RVS
        sta     CURS_X
        sta     CURS_Y

        jmp     plot            ; Set the cursor


