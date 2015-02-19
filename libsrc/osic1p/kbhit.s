;
; unsigned char kbhit (void);
;

        .export _kbhit
        .include "osic1p.inc"

_kbhit:
        lda     #%11111110      ; Select first keyboard row
scan:
        sta     KBD             ; Select keyboard row
        tax                     ; Save A
        lda     KBD             ; Read keyboard columns
        ora     #$01            ; Mask out lsb (Shift Lock), since we ignore it
        cmp     #$FF            ; No keys pressed?
        bne     keypressed
        txa                     ; Restore A
        sec                     ; Want to shift in ones
        rol     a               ; Rotate row select to next bit position
        cmp     #$FF            ; Done?
        bne     scan            ; If not, continue
        ldx     #$00            ; High byte of return is always zero
        lda     #$00            ; Return false
        rts
keypressed:
        ldx     #$00            ; High byte of return is always zero
        lda     #$01            ; Return true
        rts
