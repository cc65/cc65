;
; Ullrich von Bassewitz, 2000-08-06, 2002-12-21
; Using lots of code from MagerValp, MagerValp@cling.gu.se
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot
        .import         popa, _gotoxy
        .import         PLOT

        .include        "c128.inc"

newline         = NEWLINE

;--------------------------------------------------------------------------

.code

_cputcxy:
        pha                     ; Save C
        jsr     popa            ; Get Y
        jsr     _gotoxy         ; Set cursor, drop x
        pla                     ; Restore C

; Plot a character - also used as internal function

_cputc: cmp     #$0A            ; CR?
        beq     cr              ; Output a cr

        cmp     #$0D            ; LF?
        bne     L2
        jmp     NEWLINE         ; Update cursor position

; Printable char of some sort

L2:     cmp     #' '
        bcc     cputdirect      ; Other control char
        tay
        bmi     L5
        cmp     #$60
        bcc     L3
        and     #$DF
        bne     cputdirect      ; Branch always
L3:     and     #$3F

; Output one character to the screen. We will disable scrolling while doing so

cputdirect:
        tax                     ; Save output char
        lda     SCROLL
        pha                     ; Save scroll flag
        lda     #$C0
        sta     SCROLL          ; Disable scrolling
        txa                     ; Restore output char
        jsr     PRINT
        pla
        sta     SCROLL          ; Restore old scroll flag
        rts

; Handle character if high bit set

L5:     and     #$7F
        cmp     #$7E            ; PI?
        bne     L6
        lda     #$5E            ; Load screen code for PI
        bne     cputdirect
L6:     ora     #$40
        bne     cputdirect      ; Branch always

; Carriage return

cr:     lda     #0
        sta     CURS_X

; Set cursor position, calculate RAM pointers

plot:   ldy     CURS_X
        ldx     CURS_Y
        clc
        jmp     PLOT            ; Set the new cursor

; Write one character to the screen without doing anything else, return X
; position in Y

putchar = $CC2F

