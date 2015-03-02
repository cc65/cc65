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

FIRSTVISC       = $85           ; Offset of first visible character in video RAM
LINEDIST        = $20           ; Offset in video RAM between two lines
BLOCKSIZE       = $100          ; Size of block to scroll

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
        cpy     #(SCR_WIDTH - 1)
        bne     L3
        jsr     newline         ; New line
        ldy     #$FF            ; + cr
L3:     iny
        sty     CURS_X
        rts

newline:
        inc     CURS_Y
        lda     CURS_Y
        cmp     #SCR_HEIGHT     ; Screen height
        bne     plot
        dec     CURS_Y          ; Bottom of screen reached, scroll
        ldx     #0
scroll:
.repeat 3, I                    ; Scroll screen in three blocks of size
                                ; BLOCKSIZE
        lda     SCRNBASE+(I*BLOCKSIZE)+FIRSTVISC+LINEDIST,x
        sta     SCRNBASE+(I*BLOCKSIZE)+FIRSTVISC,x
.endrepeat
        inx
        bne scroll

        lda     #' '            ; Clear bottom line of screen
bottom:
        sta     SCRNBASE+(3*BLOCKSIZE)+FIRSTVISC,x
        inx
        cpx     #SCR_WIDTH
        bne     bottom

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
