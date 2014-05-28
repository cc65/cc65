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
        .import         popa, _gotoxy, mul20
        .importzp       ptr4
        .import         setcursor

        .constructor    screen_setup, 26
        .import         screen_setup_20x24
screen_setup    = screen_setup_20x24


_cputcxy:
        pha                     ; Save C
        jsr     popa            ; Get Y
        jsr     _gotoxy         ; Set cursor, drop x
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
        and     #$9f
        ora     ataint,x

cputdirect:                     ; accepts screen code
        jsr     putchar

; advance cursor
        inc     COLCRS_5200
        lda     COLCRS_5200
        cmp     #20
        bcc     plot
        lda     #0
        sta     COLCRS_5200

        .export newline
newline:
        inc     ROWCRS_5200
        lda     ROWCRS_5200
        cmp     #24
        bne     plot
        lda     #0
        sta     ROWCRS_5200
plot:   jsr     setcursor
        ldy     COLCRS_5200
        ldx     ROWCRS_5200
        rts

putchar:
        pha                     ; save char

        lda     ROWCRS_5200
        jsr     mul20           ; destroys tmp4
        clc
        adc     SAVMSC          ; add start of screen memory
        sta     ptr4
        txa
        adc     SAVMSC+1
        sta     ptr4+1
        pla                     ; get char again

        ldy     COLCRS_5200
        sta     (ptr4),y
        jmp     setcursor

        .rodata
ataint: .byte   64,0,32,96

