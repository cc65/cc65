;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
; Optimizations by Brandon Woodward
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;
; Important note: The implementation of cputs() relies on the cputc() function
; not clobbering ptr1. Beware when rewriting or changing this function!

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline
        .constructor    initconio
        .import         ppuinit, paletteinit, ppubuf_put
        .import         setcursor_pos, setcursor_screenptr, gotoxy

        .importzp       tmp3,tmp4

        .include        "nes.inc"

;-----------------------------------------------------------------------------

.code

_cputcxy:
        pha                     ; Save C
        jsr     gotoxy          ; Set cursor, drop x and y
        pla                     ; Restore C

; Plot a character - also used as internal function

_cputc: cmp     #$0D            ; CR?
        bne     L1
        lda     #0
        sta     CURS_X
        beq     plot            ; Recalculate pointers

L1:     cmp     #$0A            ; LF?
        beq     newline         ; Recalculate pointers

; Printable char of some sort

cputdirect:
        jsr     putchar         ; Write the character to the screen

; Advance cursor position

advance:
        ldy     CURS_X
        ldx     CURS_Y
        iny
        cpy     #xsize
        bne     L3
        inx                     ; new line
        ldy     #0              ; + cr
L3:     jmp     setcursor_pos

newline:
        inc     CURS_Y

; Calculate cursor VRAM pointer

plot:   ldy     CURS_X
        ldx     CURS_Y
        jmp     setcursor_screenptr     ; Set the new cursor


; Write one character to the screen without doing anything else, return X
; position in Y

putchar:
        ora     RVS             ; Set revers bit
        ldx     SCREEN_PTR+1
        ldy     SCREEN_PTR
        jmp     ppubuf_put

;-----------------------------------------------------------------------------
; Initialize the conio subsystem. Code goes into the ONCE segment, which may
; be reused after startup.

.segment        "ONCE"

initconio:
        ldx     #0
        ldy     #0
        sty     RVS             ; Reset revers

        jmp     setcursor_pos   ; Set the cursor position
