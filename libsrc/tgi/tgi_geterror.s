;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned char tgi_geterror (void);
; /* Return the error code for the last operation. This will also clear the
; ** error.
; */

        .include        "tgi-kernel.inc"

.proc   _tgi_geterror

        lda     _tgi_drv                ; Check if we have a driver
        ora     _tgi_drv+1
        beq     @L1
        jsr     tgi_geterror            ; First call driver
@L1:    ldx     #$00                    ; Clear high byte
        ldy     _tgi_error              ; Test high level error code
        beq     @L2                     ; Branch if no high level error code
        tya                             ; Use high level code if we have one
        stx     _tgi_error              ; Clear high level error code
@L2:    rts

.endproc
