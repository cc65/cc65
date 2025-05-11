;
; Colin Leroy-Mira, 2025-05-11
;

        .export         mouse_libref
        .import         _get_tv, ostype, return0

        .include        "get_tv.inc"

.proc mouse_libref
        lda     ostype
        cmp     #$40          ; Technical notes say not to change
        bcs     :+            ; interrupt rate on IIc/IIgs, so...
        jmp     _get_tv

:       lda     #TV::OTHER    ; ...return "Other" on those machines
        rts
.endproc
