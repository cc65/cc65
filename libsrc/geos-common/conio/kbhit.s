;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; unsigned char kbhit (void);

            .export _kbhit

            .include "geossym.inc"

_kbhit:
        ldx #0                  ; High byte of result
        lda pressFlag
        rol                     ; Bit 7 is new key flag
        txa                     ; A = 0
        rol
        rts
