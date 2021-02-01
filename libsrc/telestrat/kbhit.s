;
; Jede, 2021-02-01
;
; int kbhit (void);
;

        .export         _kbhit

        .include        "telestrat.inc"

_kbhit:
        BRK_TELEMON XRD0
        bcs     @no_char_action
        lda     #$01
        rts
@no_char_action:
        lda     #$00        
        rts

