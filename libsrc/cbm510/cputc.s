;
; Ullrich von Bassewitz, 14.09.2001
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc, cputdirect, putchar
        .export         newline, plot

        .import         popa, _gotoxy
        .import         __VIDRAM_START__
        .import         CURS_X: zp, CURS_Y: zp, CHARCOLOR: zp, RVS: zp
        .import         SCREEN_PTR: zp, CRAM_PTR: zp

        .include        "cbm510.inc"

        .macpack        generic

; ------------------------------------------------------------------------
;

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

; Handle character if high bit set

L10:    and     #$7F
        cmp     #$7E            ; PI?
        bne     L11
        lda     #$5E            ; Load screen code for PI
        bne     cputdirect
L11:    ora     #$40
        bne     cputdirect      ; Branch always

; Move the cursor into the next line

newline:
        inc     CURS_Y

; Set cursor position, calculate RAM pointers

plot:   ldx     CURS_Y
        lda     LineLSBTab,x
        sta     SCREEN_PTR
        sta     CRAM_PTR
        lda     LineMSBTab,x
        sta     SCREEN_PTR+1
        add     #.hibyte(COLOR_RAM - __VIDRAM_START__)
        sta     CRAM_PTR+1
        rts

; Write one character to the screen without doing anything else, return X
; position in Y

putchar:
        ora     RVS             ; Set revers bit
        ldy     CURS_X
        sta     (SCREEN_PTR),y  ; Set char
        ldx     IndReg
        lda     #$0F
        sta     IndReg
        lda     CHARCOLOR
        sta     (CRAM_PTR),y    ; Set color
        stx     IndReg
        rts

; -------------------------------------------------------------------------
; Low bytes of the start address of the screen lines

.rodata

LineLSBTab:
        .repeat 25, I
        .byte   .lobyte(__VIDRAM_START__ + I * 40)
        .endrep

; -------------------------------------------------------------------------
; High bytes of the start address of the screen lines

LineMSBTab:
        .repeat 25, I
        .byte   .hibyte(__VIDRAM_START__ + I * 40)
        .endrep
