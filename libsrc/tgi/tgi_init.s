;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_init (unsigned char mode);
; /* Initialize the given graphics mode. */


        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .import         _tgi_done
        .export         _tgi_init

_tgi_init:
        pha                             ; Save mode
        jsr     _tgi_done               ; Switch off graphics if needed
        jsr     tgi_init                ; Initialize the mode
        jsr     tgi_fetch_error         ; Get the error code
        pla                             ; Restore the mode
        ldx     _tgi_error              ; Did we have an error before?
        bne     @L1                     ; Jump if yes
        sta     _tgi_mode               ; Set the current mode if not
@L1:    rts

