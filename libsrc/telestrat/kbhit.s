;
; Jede, 2021-02-01
;
; int kbhit (void);
;

        .export         _kbhit

        .export         store_char

        .include        "telestrat.inc"

_kbhit:
        BRK_TELEMON XRD0
        ldx     #$00 
        bcs     @no_char_action
        sta     store_char
        lda     #$01
        rts
@no_char_action:
        tax        
        rts
.data        
store_char:
        .byte 0
