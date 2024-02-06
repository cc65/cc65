;
; Ullrich von Bassewitz, 25.11.2002
;
; CC65 runtime: Swap x bytes of register variable space
;

        .export         regswap
        .importzp       sp, regbank, tmp1

.proc   regswap

        sta     tmp1                    ; Store count
@L1:    lda     regbank,x               ; Get old value
        pha                             ; Save it
        lda     (sp),y                  ; Get stack loc
        sta     regbank,x               ; Store new value
        pla
        sta     (sp),y                  ; Store old value
        inx
        iny
        dec     tmp1
        bne     @L1

        rts

.endproc


