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
        pla
        sta     _tgi_mode               ; Remember the mode
        jsr     tgi_init                ; Go into graphics mode
        jsr     tgi_fetch_error         ; Get the error code
        beq     @L1                     ; Jump if no error
        lda     #$00
        sta     _tgi_mode               ; Clear the mode if init was not successful
@L1:    rts

