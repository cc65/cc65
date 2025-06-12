;
; Colin Leroy-Mira <colin@colino.net>, 2023-09-06
;
; int __fastcall__ ntohl (long val);
;

.export     _ntohl, _htonl
.import     popa
.importzp   tmp1, tmp2, sreg

_htonl := _ntohl

_ntohl:
        ; The parts of our 32 bit word
        ; are in sreg+1, sreg, X, A.

        ; Save A and X
        stx tmp1
        sta tmp2

        ; Invert high word
        lda sreg+1
        ldx sreg

        ; Invert low word
        ldy tmp1
        sty sreg

        ldy tmp2
        sty sreg+1

        rts
