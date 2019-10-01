;
; adapted from Atari version
; Christian Groessler, 2014
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .include        "atari5200.inc"

        .export         _cputcxy, _cputc
        .export         plot, cputdirect, putchar
        .import         gotoxy, _mul20
        .import         conio_color
        .importzp       screen_width, screen_height
        .importzp       ptr4

        .import         screen_setup
        .constructor    initconio
initconio               =       screen_setup

_cputcxy:
        pha                     ; Save C
        jsr      gotoxy         ; Set cursor, drop x and y
        pla                     ; Restore C

_cputc:
        cmp     #$0D            ; CR
        bne     L4
        lda     #0
        sta     COLCRS_5200
        beq     plot            ; return

L4:     cmp     #$0A            ; LF
        beq     newline
        cmp     #ATEOL          ; Atari-EOL?
        beq     newline

        tay
        rol     a
        rol     a
        rol     a
        rol     a
        and     #3
        tax
        tya
        and     #$9F
        ora     ataint,x

cputdirect:                     ; accepts screen code
        jsr     putchar

; advance cursor
        inc     COLCRS_5200
        lda     COLCRS_5200
        cmp     #screen_width
        bcc     plot
        lda     #0
        sta     COLCRS_5200

        .export newline
newline:
        inc     ROWCRS_5200
        lda     ROWCRS_5200
        cmp     #screen_height
        bne     plot
        lda     #0
        sta     ROWCRS_5200
plot:   ldy     COLCRS_5200
        ldx     ROWCRS_5200
        rts

putchar:
        pha                     ; save char

        lda     ROWCRS_5200
        jsr     _mul20          ; destroys tmp4, carry is cleared
        adc     SAVMSC          ; add start of screen memory
        sta     ptr4
        txa
        adc     SAVMSC+1
        sta     ptr4+1
        pla                     ; get char again

        and     #$3F            ; clear palette index bits
        ora     conio_color     ; use currently selected palette

        ldy     COLCRS_5200
        sta     (ptr4),y
        rts

        .rodata
ataint: .byte   64,0,32,96
