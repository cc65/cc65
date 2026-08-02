;
; Robert Justice
;
; char cgetc (void);
;

        .export         _cgetc, getchar
        .import         cursor, _cputc, consref

        .include        "apple3.inc"
        .include        "sos.inc"

_cgetc:
        ; Cursor on ?
        lda     cursor
        beq     :+


        lda     #CONSOLE_CURSOR_ON
        jsr     _cputc
:
        jsr     getchar
        
        ; Cursor on ?
        ldy     cursor
        beq     :+

        pha
        lda     #CONSOLE_CURSOR_OFF
        jsr     _cputc
        pla
:       
        ldx     #>$0000
        rts


        ; Read key from console
getchar:
        lda     consref
        sta     read_chref
        brk
        .byte   READ_CALL
        .addr   read_char

        lda     readbuf
        rts


        .data

read_char:
        .byte   $04
read_chref:
        .byte   $00
        .addr   readbuf
        .word   $0001           ; REQUEST_COUNT
        .word   $0000           ; TRANS_COUNT

readbuf:
        .byte 1