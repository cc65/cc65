;
; cputc/cputcxy for Challenger 1P
; Originally based on PET/CBM implementation
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;
        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot
        .import         popa, _gotoxy
        .import         ScrLo, ScrHi, ScrollLength, ScrollDist, ScrFirstChar
        .import         ScrWidth, ScrHeight
        .import         _memmove, pushax
        .importzp       tmp2, ptr1
                                ; tmp1 is used by cputs!

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

; Advance cursor position, register Y contains horizontal position after
; putchar

        ldx     ScrWidth        ; Check whether line is full
        dex
        stx     tmp2
        cpy     tmp2
        bne     L3
        jsr     newline         ; New line
        ldy     #$FF            ; + cr
L3:     iny
        sty     CURS_X
        rts

newline:
        inc     CURS_Y
        lda     CURS_Y
        cmp     ScrHeight       ; Screen height
        bne     plot
        dec     CURS_Y          ; Bottom of screen reached, scroll

        lda     ScrFirstChar    ; Scroll destination address
        ldx     ScrFirstChar+1
        jsr     pushax
        clc                     ; Compute from address by adding the
        adc     ScrollDist      ; distance for one line in video RAM
        sta     tmp2
        txa
        adc     #0
        tax
        lda     tmp2
        jsr     pushax
        lda     ScrollLength    ; Distance for shifting by one line
        ldx     ScrollLength+1
        jsr     _memmove

        clc                     ; Compute address of first character
        lda     ScrFirstChar    ; in last line of screen
        adc     ScrollLength
        sta     ptr1
        lda     ScrFirstChar+1
        adc     ScrollLength+1
        sta     ptr1+1

        ldy     ScrWidth        ; Fill line with blanks
        lda     #' '
clrln:  sta     (ptr1),y
        dey
        bpl     clrln

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
