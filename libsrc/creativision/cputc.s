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
        .import         setcursor

        .importzp       tmp3,tmp4

        .include        "creativision.inc"
        .include        "boxchars.inc"

;-----------------------------------------------------------------------------

.code

_cputcxy:
        pha                     ; Save C
        jsr     popa            ; Get Y
        jsr     _gotoxy         ; Set cursor, drop x
        pla                     ; Restore C

; Plot a character - also used as internal function

_cputc: cmp     #$0D            ; CR?
        bne     L1
        lda     #0
        sta     CURSOR_X
        beq     plot            ; Recalculate pointers

L1:     cmp     #$0A            ; LF?
        beq     newline         ; Recalculate pointers

; Printable char of some sort

cputdirect:
        jsr     putchar         ; Write the character to the screen

; Advance cursor position

advance:
        ldy     CURSOR_X
        iny
        cpy     #SCREEN_COLS
        bne     L3
        inc     CURSOR_Y        ; new line
        ldy     #0              ; + cr
L3:     sty     CURSOR_X
        jmp     plot

newline:
        inc     CURSOR_Y

; Set cursor position, calculate RAM pointers

plot:   ldy     CURSOR_X
        ldx     CURSOR_Y
        jmp     setcursor       ; Set the new cursor


; Write one character to the screen without doing anything else, return X
; position in Y

putchar:
        cmp     #$5B
        bcc     IS_UPPER

        clc
        sbc     #$1F

IS_UPPER:
        cmp     #$20
        bcc     BAD_CHAR

        pha
        lda     SCREEN_PTR
        sei
        sta     VDP_CONTROL_W
        lda     SCREEN_PTR+1
        ora     #$40
        sta     VDP_CONTROL_W
        pla
        clc
        adc     #160
        sta     VDP_DATA_W
        cli

BAD_CHAR:
        jmp     plot

initconio:
        lda     #$0
        sta     SCREEN_PTR
        lda     #$10
        sta     SCREEN_PTR+1

        ; Copy box characters to slot
        sei
        lda     #08
        sta     VDP_CONTROL_W
        lda     #$46
        sta     VDP_CONTROL_W
        ldx     #0

LL:     lda     boxchars,x
        sta     VDP_DATA_W
        inx
        cpx     #48
        bne     LL

        cli
        jmp     plot
