;
; Marco van den Heuvel, 2018-03-28
;
; void slow (void);
; /* Switch the CPU into single clock mode. */
;

        .export         _slow

        .include        "plus4.inc"


.proc   _slow

        lda     TED_CLK
        ora     #%00000010
        sta     TED_CLK
        rts

.endproc


