;
; Jede, 2021-02-01
;
; int kbhit (void);
;

        .export         _kbhit

        .import         store_char

        .include        "telestrat.inc"

_kbhit:
        lda     store_char       ; Check if a key has been detected previously
        beq     @call_telemon    ; No, calls Telemon routine
        lda     #$01             ; There is a key pressed previously, return 1
        ldx     #$00
        rts
@call_telemon:        
        BRK_TELEMON XRD0

        ldx     #$00
        bcs     @no_char_action
        sta     store_char
        lda     #$01
        rts
@no_char_action:
        tax
        rts
