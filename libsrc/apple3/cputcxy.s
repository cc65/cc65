;
; Robert Justice, 07.06.2026
;
; void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
; void __fastcall__ gotoxy (unsigned char x, unsigned char y);
; void __fastcall__ gotox (unsigned char x);
; void __fastcall__ gotoy (unsigned char y);
;

        .export         _cputcxy, _gotoxy, _gotox, _gotoy, gotoxy
        .import         consref, popa ;, newline, putchar, putchardirect

        .include        "apple3.inc"
        .include        "sos.inc"

        .code


_cputcxy:
        sta     xybuf+4
        jsr     popa            ; Get Y
        sta     xybuf+3
        jsr     popa            ; Get X
        sta     xybuf+1
        lda     consref
        sta     putcxyref     
        brk
        .byte   WRITE_CALL
        .word   putcxycon
        rts

gotoxy:
        jsr     popa            ; Get Y

_gotoxy:
        sta     xybuf+3
        jsr     popa            ; Get X
        sta     xybuf+1
        lda     consref
        sta     gotoxyref     
        brk
        .byte   WRITE_CALL
        .word   gotoxycon
        rts

_gotox:
        sta     xybuf+1
        lda     consref
        sta     gotoxref     
        brk
        .byte   WRITE_CALL
        .word   gotoxcon
        rts

_gotoy:
        sta     ybuf+1
        lda     consref
        sta     gotoyref     
        brk
        .byte   WRITE_CALL
        .word   gotoycon
        rts

        .data

; cputcxy param list
putcxycon:
        .byte   3
putcxyref:
        .byte   0
        .word   xybuf
        .word   5

; gotoxy param list
gotoxycon:
        .byte   3
gotoxyref:
        .byte   0
        .word   xybuf
        .word   4

; gotox param list
gotoxcon:
        .byte   3
gotoxref:
        .byte   0
        .word   xybuf
        .word   2

; gotoy param list
gotoycon:
        .byte   3
gotoyref:
        .byte   0
        .word   ybuf
        .word   2


xybuf:  .byte   24  ; horizontal pso
        .byte   00
ybuf:   .byte   25  ; vertical pos
        .byte   00
        .byte   00