;
; Robert Justice, 2026
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

        .export         _textcolor, _bgcolor, _bordercolor
        .import         return0, consref

        .include        "apple3.inc"
        .include        "sos.inc"

_textcolor:
        sta     colbuf+1
        ldx     consforecolor
        sta     consforecolor
        txa
        pha
        lda     #CONSOLE_FORE_COLOR
        sta     colbuf
        bne     :+

_bgcolor:
        sta     colbuf+1
        ldx     consforecolor
        ldx     consforecolor
        txa
        pha
        lda     #CONSOLE_BACK_COLOR
        sta     colbuf

:       lda     consref
        sta     colref
        brk
        .byte   WRITE_CALL
        .addr   collist
        pla
        rts

_bordercolor    := return0


        .data

; write console param list
collist:  .byte   3
colref:   .byte   0
          .addr   colbuf
          .word   2

colbuf:   .byte   00
          .byte   00

consforecolor:
        .byte   15
consbackcolor:
        .byte   0

