;
; Ullrich von Bassewitz, 06.08.1998
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot
        .import         popa, _gotoxy

        .include        "c1p.inc"
        .include        "extzp.inc"

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

; Printable char of some sort

;        cmp     #' '
;        bcc     cputdirect      ; Other control char < 0x20
;        tay
;        bmi     L10
;        cmp     #$60
;        bcc     L2
;        and     #$DF
;        bne     cputdirect      ; Branch always
;L2:     and     #$3F

cputdirect:
        jsr     putchar         ; Write the character to the screen

; Advance cursor position

advance:
        cpy     SCR_LINELEN     ; xsize-1
        bne     L3
        jsr     newline         ; new line
        ldy     #$FF            ; + cr
L3:     iny
        sty     CURS_X
        rts

newline:
        lda     SCR_LINELEN     ; xsize-1
        sec                     ; Account for -1 above
        adc     SCREEN_PTR
        sta     SCREEN_PTR
        bcc     L4
        inc     SCREEN_PTR+1
L4:     inc     CURS_Y
        rts

; Handle character if high bit set

; L10:    and     #$7F
;        cmp     #$7E            ; PI?
;        bne     L11
;        lda     #$5E            ; Load screen code for PI
;        bne     cputdirect
; L11:    ora     #$40
;        bne     cputdirect



; Set cursor position, calculate RAM pointers

plot:   ldy     CURS_Y
        lda     ScrLo,y
        sta     SCREEN_PTR
        lda     ScrHi,y
;        ldy     SCR_LINELEN
;        cpy     #40+1
;        bcc     @L1
;        asl     SCREEN_PTR              ; 80 column mode
;        rol     a
;@L1:    ora     #$80                    ; Screen at $8000
        sta     SCREEN_PTR+1
        rts


; Write one character to the screen without doing anything else, return X
; position in Y

putchar:
;        ora     RVS             ; Set revers bit
        ldy     CURS_X
        sta     (SCREEN_PTR),y  ; Set char
        rts

; Screen address tables - offset to real screen

.rodata

ScrLo:  .byte   $83, $A3, $C3, $E3, $03, $23, $43, $63
        .byte   $83, $A3, $C3, $E3, $03, $23, $43, $63
        .byte   $83, $A3, $C3, $E3, $03, $23, $43, $63
        .byte   $83

ScrHi:  .byte   $D0, $D0, $D0, $D0, $D1, $D1, $D1, $D1
        .byte   $D1, $D1, $D1, $D1, $D2, $D2, $D2, $D2
        .byte   $D2, $D2, $D2, $D2, $D3, $D3, $D3, $D3
        .byte   $D3
