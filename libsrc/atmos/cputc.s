;
; 2003-04-13, Ullrich von Bassewitz
; 2013-07-16, Greg King
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc
        .export         setscrptr, putchar
        .import         rvs
        .import         popax
        .importzp       ptr2

        .include        "atmos.inc"


_cputcxy:
        pha                     ; Save C
        jsr     popax           ; Get X and Y
        sta     CURS_Y          ; Store Y
        stx     CURS_X          ; Store X
        pla                     ; Restore C

; Plot a character - also used as internal function

_cputc: cmp     #$0D            ; CR?
        bne     L1
        lda     #0
        sta     CURS_X          ; Carriage return
        rts

L1:     cmp     #$0A            ; LF?
        bne     output
        inc     CURS_Y          ; Newline
        rts

; Output the character, then advance the cursor position

output:
        jsr     putchar

advance:
        iny
        cpy     #SCREEN_XSIZE
        bne     L3
        inc     CURS_Y          ; new line
        ldy     #0              ; + cr
L3:     sty     CURS_X
        rts

; ------------------------------------------------------------------------
; Set ptr2 to the screen, load the X offset into Y

.code
.proc   setscrptr

        ldy     CURS_Y          ; Get line number into Y
        lda     ScrTabLo,y      ; Get low byte of line address
        sta     ptr2
        lda     ScrTabHi,y      ; Get high byte of line address
        sta     ptr2+1
        ldy     CURS_X          ; Get X offset
        rts

.endproc

; ------------------------------------------------------------------------
; Write one character to the screen without doing anything else, return X
; position in Y

.code
.proc   putchar

        ora     rvs             ; Set revers bit
        pha                     ; And save
        jsr     setscrptr       ; Set ptr2 to the screen
        pla                     ; Restore the character
        sta     (ptr2),y        ; Set char
        rts

.endproc

; ------------------------------------------------------------------------
; Screen address table

.rodata
ScrTabLo:
        .repeat SCREEN_YSIZE, Line
                .byte   <(SCREEN + Line * SCREEN_XSIZE)
        .endrep

ScrTabHi:
        .repeat SCREEN_YSIZE, Line
                .byte   >(SCREEN + Line * SCREEN_XSIZE)
        .endrep

