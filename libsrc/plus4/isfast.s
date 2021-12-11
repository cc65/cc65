;
; Marco van den Heuvel, 2018-03-20
;
; unsigned char isfast (void);
; /* Returns 1 if the CPU is in double clock mode. */
;

        .export         _isfast

        .include        "plus4.inc"


.proc   _isfast

        lda     TED_CLK
        lsr
        and     #$01
        ldx     #$00
        rts

.endproc

