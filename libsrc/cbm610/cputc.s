;
; Ullrich von Bassewitz, 06.08.1998
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot
        .destructor     setsyscursor

        .import         _gotoxy
        .import         popa
        .import         PLOT

        .import         ktmp: zp, crtc: zp, CURS_X: zp, CURS_Y: zp, RVS: zp
        .import         CharPtr: zp

        .include        "cbm610.inc"


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

        cmp     #' '
        bcc     cputdirect      ; Other control char
        tay
        bmi     L10
        cmp     #$60
        bcc     L2
        and     #$DF
        bne     cputdirect      ; Branch always
L2:     and     #$3F

cputdirect:
        jsr     putchar         ; Write the character to the screen

; Advance cursor position

advance:
        iny
        cpy     #XSIZE
        bne     L3
        jsr     newline         ; new line
        ldy     #0              ; + cr
L3:     sty     CURS_X
        rts

newline:
        clc
        lda     #XSIZE
        adc     CharPtr
        sta     CharPtr
        bcc     L4
        inc     CharPtr+1
L4:     inc     CURS_Y
        rts

; Handle character if high bit set

L10:    and     #$7F
        cmp     #$7E            ; PI?
        bne     L11
        lda     #$5E            ; Load screen code for PI
        bne     cputdirect
L11:    ora     #$40
        bne     cputdirect      ; Branch always

; Write one character to the screen without doing anything else, return X
; position in Y

putchar:
        ldx     IndReg
        ldy     #$0F
        sty     IndReg
        ora     RVS             ; Set revers bit
        ldy     CURS_X
        sta     (CharPtr),y     ; Set char
        stx     IndReg
        rts

; Set cursor position, calculate RAM pointers

plot:   ldx     CURS_Y
        lda     LineLSBTab,x
        sta     CharPtr
        lda     LineMSBTab,x
        sta     CharPtr+1

        lda     IndReg
        pha
        lda     #$0F
        sta     IndReg

        ldy     #$00
        clc
        sei
        sta     (crtc),y
        lda     CharPtr
        adc     CURS_X
        iny
        sta     (crtc),y
        dey
        lda     #$0E
        sta     (crtc),y
        iny
        lda     (crtc),y
        and     #$F8
        sta     ktmp
        lda     CharPtr+1
        adc     #$00
        and     #$07
        ora     ktmp
        sta     (crtc),y
        cli

        pla
        sta     IndReg
        rts

; -------------------------------------------------------------------------
; Cleanup routine that sets the kernal cursor position to ours

.segment        "PAGE2"

setsyscursor:
        ldy     CURS_X
        ldx     CURS_Y
        clc
        jmp     PLOT            ; Set the new cursor

; -------------------------------------------------------------------------
; Low bytes of the start address of the screen lines

.rodata

LineLSBTab:
        .byte   $00,$50,$A0,$F0,$40,$90,$E0,$30
        .byte   $80,$D0,$20,$70,$C0,$10,$60,$B0
        .byte   $00,$50,$A0,$F0,$40,$90,$E0,$30
        .byte   $80
; -------------------------------------------------------------------------
; High bytes of the start address of the screen lines

LineMSBTab:
        .byte   $D0,$D0,$D0,$D0,$D1,$D1,$D1,$D2
        .byte   $D2,$D2,$D3,$D3,$D3,$D4,$D4,$D4
        .byte   $D5,$D5,$D5,$D5,$D6,$D6,$D6,$D7
        .byte   $D7

