;
; Ullrich von Bassewitz, 22.11.2002
;
; READST replacement function
;

        .export         READST

        .include        "plus4.inc"
        .importzp       ST

; Read the status byte from the zero page instead of banking in the ROM

.proc   READST
        lda     ST                      ; Load status
        rts                             ; Return to caller
.endproc


