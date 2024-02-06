;
; unsigned char __fastcall__ kbrepeat (unsigned char mode);
;

        .export _kbrepeat

        .include        "plus4.inc"

_kbrepeat:
        ldx KBDREPEAT           ; get old value
        sta KBDREPEAT           ; store new value
        txa                     ; return old value
        ldx #0
        rts
