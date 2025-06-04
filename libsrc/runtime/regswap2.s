;
; Ullrich von Bassewitz, 25.11.2002
;
; CC65 runtime: Swap 2 bytes of register variable space
;

        .export         regswap2
        .importzp       spc, regbank

.proc   regswap2

; First byte

        lda     regbank,x               ; Get old value
        pha                             ; Save it
        lda     (spc),y                  ; Get stack loc
        sta     regbank,x               ; Store new value
        pla
        sta     (spc),y                  ; Store old value

; Second byte

        iny
        lda     regbank+1,x             ; Get old value
        pha                             ; Save it
        lda     (spc),y                  ; Get stack loc
        sta     regbank+1,x             ; Store new value
        pla
        sta     (spc),y                  ; Store old value

        rts

.endproc


