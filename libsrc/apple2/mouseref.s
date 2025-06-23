;
; Colin Leroy-Mira, 2025-05-11
;

        .export         mouse_libref
        .import         _get_tv, ostype, return0

        .constructor    init_mousetv

        .include        "get_tv.inc"

        .segment "ONCE"

.proc init_mousetv
        lda     ostype
        cmp     #$40          ; Technical notes say not to change
        bcs     :+            ; interrupt rate on IIc/IIgs, so...
        jsr     _get_tv
        sta     mouse_libref
:       rts                   ; ...don't update "Other" on those machines
.endproc

        .data

mouse_libref: .byte TV::OTHER
