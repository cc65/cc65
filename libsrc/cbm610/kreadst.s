;
; Ullrich von Bassewitz, 2003-12-19
;
; READST kernal call
;

        .export         READST

        .include        "cbm610.inc"


.proc   READST

        lda     ST                      ; Load status
        rts                             ; Return to caller

.endproc


