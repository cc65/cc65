;
; Mark Keates, Christian Groessler, Piotr Fusik
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputcxy, _cputc
        .export         plot, cputdirect, putchar
        .import         gotoxy, _mul40
        .importzp       tmp4,ptr4
        .import         _revflag,setcursor

        .include        "atari.inc"

_cputcxy:
        pha                     ; Save C
        jsr     gotoxy          ; Set cursor, drop x and y
        pla                     ; Restore C

_cputc:
        cmp     #$0D            ; CR
        bne     L4
        lda     #0
        sta     COLCRS
        beq     plot            ; return

L4:     cmp     #$0A            ; LF
        beq     newline
        cmp     #ATEOL          ; Atari-EOL?
        beq     newline

        asl     a               ; shift out the inverse bit
        adc     #$c0            ; grab the inverse bit; convert ATASCII to screen code
        bpl     codeok          ; screen code ok?
        eor     #$40            ; needs correction
codeok: lsr     a               ; undo the shift
        bcc     cputdirect
        eor     #$80            ; restore the inverse bit

cputdirect:                     ; accepts screen code
        jsr     putchar

; advance cursor
        inc     COLCRS
        lda     COLCRS
        cmp     #40
        bcc     plot
        lda     #0
        sta     COLCRS

        .export newline
newline:
        inc     ROWCRS
        lda     ROWCRS
        cmp     #24
        bne     plot
        lda     #0
        sta     ROWCRS
plot:   jsr     setcursor
        ldy     COLCRS
        ldx     ROWCRS
        rts

; turn off cursor, update screen, turn on cursor
putchar:
        pha                     ; save char

        ldy     #0
        lda     OLDCHR
        sta     (OLDADR),y

        lda     ROWCRS
        jsr     _mul40          ; destroys tmp4, carry is cleared
        adc     SAVMSC          ; add start of screen memory
        sta     ptr4
        txa
        adc     SAVMSC+1
        sta     ptr4+1
        pla                     ; get char again

        ora     _revflag
        sta     OLDCHR

        ldy     COLCRS
        sta     (ptr4),y
        jmp     setcursor
