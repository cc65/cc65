;
; Jede, 2021-02-01
;
; int kbhit (void);
;

        .export         _kbhit

        .include        "telestrat.inc"

_kbhit:
        BRK_TELEMON XRD0
        ldx     #$00
        txa
        rol
        eor     #$01
        rts
