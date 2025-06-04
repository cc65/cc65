;
; Ullrich von Bassewitz, 25.11.2002
;
; CC65 runtime: Swap 1 byte of register variable space
;

        .export         regswap1
        .importzp       spc, regbank

.proc   regswap1

        lda     regbank,x               ; Get old value
        pha                             ; Save it
        lda     (spc),y                  ; Get stack loc
        sta     regbank,x               ; Store new value
        pla
        sta     (spc),y                  ; Store old value
        rts

.endproc


