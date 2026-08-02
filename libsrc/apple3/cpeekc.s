;
; 2020-07-12, Oliver Schmidt
; Robert Justice, 2026
;
; char cpeekc (void);
;

        .export         _cpeekc
        .import         statcons

        .include        "apple3.inc"

_cpeekc:
        lda     #STAT_READ_SCR
        jmp     statcons
