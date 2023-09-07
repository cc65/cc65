;
; Colin Leroy-Mira <colin@colino.net>, 2023-09-06
;
; int __fastcall__ ntohs (int val);
;

.export     _ntohs, _htons
.importzp   tmp1

_htons := _ntohs

_ntohs:
        sta     tmp1
        txa
        ldx     tmp1
        rts
