;
; Marco van den Heuvel, 2018-03-19
;
; unsigned char isfast (void);
; /* Returns 1 if the CPU is in 2MHz mode. */
;

        .export         _isfast

        .include        "c128.inc"


.proc   _isfast

        lda     VIC_CLK_128
        and     #$01
        ldx     #$00
        rts

.endproc

