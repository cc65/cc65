;
; Ullrich von Bassewitz, 25.11.2002
;
; CC65 runtime: Swap 2 bytes of register variable space
;

        .export         regswap2
        .importzp       c_sp, regbank

.proc   regswap2

; First byte

        lda     regbank,x               ; Get old value
        pha                             ; Save it
        lda     (c_sp),y                  ; Get stack loc
        sta     regbank,x               ; Store new value
        pla
        sta     (c_sp),y                  ; Store old value

; Second byte

        iny
        lda     regbank+1,x             ; Get old value
        pha                             ; Save it
        lda     (c_sp),y                  ; Get stack loc
        sta     regbank+1,x             ; Store new value
        pla
        sta     (c_sp),y                  ; Store old value

        rts

.endproc


