;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned char __fastcall__ tgi_geterror (void);
; /* Return the error code for the last operation. This will also clear the
;  * error.
;  */

        .include        "tgi-kernel.inc"

        .export         _tgi_geterror

_tgi_geterror:
        jsr     tgi_geterror            ; First call driver
        ldx     #$00                    ; Clear high byte
        ldy     _tgi_error              ; Test high level error code
        beq     @L1                     ; Branch if no high level error code
        tya                             ; Use high level code if we have one
        stx     _tgi_error              ; Clear high level error code
@L1:    rts

