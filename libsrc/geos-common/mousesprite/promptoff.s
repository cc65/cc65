;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void PromptOff (void);

            .export _PromptOff

            .include "jumptab.inc"
            .include "geossym.inc"

_PromptOff:
; from 'The Hitchhiker's Guide To GEOS'
        php
        sei
        jsr PromptOff
        lda #0
        sta alphaFlag
        plp
        rts
