;
; Ullrich von Bassewitz, 2003-02-13
;
; void fast (void);
; /* Switch the CPU into 2MHz mode. Note: This will disable video when in
; ** 40 column mode.
; */
;

        .export         _fast

        .include        "c128.inc"


.proc   _fast

        lda     #$01
        sta     VIC_CLK_128
        rts

.endproc


