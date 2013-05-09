;
; Ullrich von Bassewitz, 17.06.1998
;
; long __fastcall__ labs (long val);
; intmax_t __fastcall imaxabs (intmax_t val);
;

        .export         _labs, _imaxabs
        .import         negeax
        .importzp       sreg


_labs:
_imaxabs:
        ldy     sreg+1          ; test hi byte
        bpl     L1
        jmp     negeax          ; Negate if negative
L1:     rts

