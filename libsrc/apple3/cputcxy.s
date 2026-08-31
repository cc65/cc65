;
; Robert Justice, 07.06.2026
;
; void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
; void __fastcall__ gotoxy (unsigned char x, unsigned char y);
; void __fastcall__ gotox (unsigned char x);
; void __fastcall__ gotoy (unsigned char y);
;

        .export         _cputcxy, _gotoxy, _gotox, _gotoy, gotoxy
        .export         xyref, yref
        .import         consref, popa
        .import         setconioscr, consscrflg

        .include        "apple3.inc"
        .include        "sos.inc"

        .code


_cputcxy:
        sta     xybuf+4
        jsr     popa            ; Get Y
        sta     xybuf+3
        jsr     popa            ; Get X
        sta     xybuf+1

        bit     consscrflg    ; check if scroll is off
        beq     :+
        jsr     setconioscr
:
        lda     #5
        bne     dowrite         ; bra

gotoxy:
        jsr     popa            ; Get Y

_gotoxy:
        sta     xybuf+3
        jsr     popa            ; Get X
        sta     xybuf+1
        lda     #4
        bne     dowrite         ; bra

_gotox:
        sta     xybuf+1
        lda     #2

dowrite:
        sta     xycnt
        brk
        .byte   WRITE_CALL
        .word   xycon
        rts

_gotoy:
        sta     ybuf+1
        brk
        .byte   WRITE_CALL
        .word   ycon
        rts


        .data

; xy param list
xycon:  .byte   3
xyref:  .byte   0
        .word   xybuf
xycnt:  .word   0

; gotoy param list
ycon:   .byte   3
yref:   .byte   0
        .word   ybuf
        .word   2

xybuf:  .byte   24  ; horizontal pso
        .byte   00
ybuf:   .byte   25  ; vertical pos
        .byte   00
        .byte   00
