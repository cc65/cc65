
        .export _kbrepeat, _kbrepeatdelay, _kbrepeatrate

        .include        "c64/c64.inc"

_kbrepeat:
        ldx KBDREPEAT           ; get old value
        sta KBDREPEAT           ; store new value
        txa                     ; return old value
        rts

_kbrepeatdelay:
        ldx KBDREPEATDELAY      ; get old value
        sta KBDREPEATDELAY      ; store new value
        txa                     ; return old value
        rts

_kbrepeatrate:
        ldx KBDREPEATRATE       ; get old value
        sta KBDREPEATRATE       ; store new value
        txa                     ; return old value
        rts
