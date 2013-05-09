;
; Maciej 'YTM/Elysium' Witkowiak
;
; 30.10.99, 20.08.2003

; void * ClearRam         (char *dest, int length);

            .import DoublePop
            .export _ClearRam

            .include "jumptab.inc"
            .include "geossym.inc"

_ClearRam:
        jsr DoublePop
        pha
        txa
        pha
        jsr ClearRam
        pla
        tax
        pla
        rts

