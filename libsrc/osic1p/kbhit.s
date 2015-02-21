;
; unsigned char kbhit (void);
;
; The method to detect a pressed key is based on the documentation in
; "Section 3 Programmed Key Functions" in "The Challenger Character Graphics
; Reference Manual"
; We only want to return true for characters that can be returned by cgetc(),
; but not for keys like <Shift> or <Ctrl>. Therefore a special handling is
; needed for the first row. This is implemented by a bit mask that is stored
; in tmp1 and that is set to zero after the first round.
;

        .export _kbhit
        .include "osic1p.inc"
        .include "extzp.inc"
        .include "zeropage.inc"

_kbhit:
        lda     #%11011111      ; Mask for only checking the column for the
        sta     tmp1            ; ESC key in the first keyboard row.

        lda     #%11111110      ; Mask for first keyboard row
scan:
        sta     KBD             ; Select keyboard row
        tax                     ; Save A
        lda     KBD             ; Read keyboard columns
        ora     tmp1            ; Mask out uninteresting keys (only relevant in
                                ; first row)
        cmp     #$FF            ; No keys pressed?
        bne     keypressed
        lda     #$00            ; For remaining rows no keys masked
        sta     tmp1
        txa                     ; Restore A
        sec                     ; Want to shift in ones
        rol     a               ; Rotate row select to next bit position
        cmp     #$FF            ; Done?
        bne     scan            ; If not, continue
        lda     #$00            ; Return false
        tax                     ; High byte of return is also zero
        sta     CHARBUF         ; No character in buffer
        rts
keypressed:
        jsr     INPUTC          ; Get input character in A
        sta     CHARBUF         ; Save in buffer
        ldx     #$00            ; High byte of return is always zero
        lda     #$01            ; Return true
        rts
