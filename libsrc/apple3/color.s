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
        lda     #CONSOLE_FORE_COLOR
        sta     colbuf
        bne     :+

_bgcolor:
        sta     colbuf+1
        lda     #CONSOLE_BACK_COLOR
        sta     colbuf
        bne     :+

_bordercolor    := return0


:       lda     consref
        sta     colref
        brk
        .byte   WRITE_CALL
        .addr   collist
        rts

        .data

; write console param list
collist:  .byte   3
colref:   .byte   0
          .addr   colbuf
          .word   2

colbuf:   .byte   00
          .byte   00
