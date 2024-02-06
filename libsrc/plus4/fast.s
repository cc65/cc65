;
; Marco van den Heuvel, 2018-03-20
;
; void fast (void);
; /* Switch the CPU into double clock mode. */
;

        .export         _fast

        .include        "plus4.inc"


.proc   _fast

        lda     TED_CLK
        and     #%11111101
        sta     TED_CLK
        rts

.endproc


