;
; Maciej 'YTM/Elysium' Witkowiak
;
; 2.1.2003
;

            .export setoserror
            .import __oserror

setoserror:
        stx __oserror
        txa
        ldx #0                  ; X is cleared (high byte for promoting char to int)
        tay                     ; Y register is used just to save flags state
        rts
