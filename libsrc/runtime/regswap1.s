;
; Ullrich von Bassewitz, 25.11.2002
;
; CC65 runtime: Swap 1 byte of register variable space
;

        .export         regswap1
        .importzp       c_sp, regbank

.proc   regswap1

        lda     regbank,x               ; Get old value
        pha                             ; Save it
        lda     (c_sp),y                  ; Get stack loc
        sta     regbank,x               ; Store new value
        pla
        sta     (c_sp),y                  ; Store old value
        rts

.endproc


