;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_init (unsigned char mode);
; /* Initialize the given graphics mode. */


        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .import         _tgi_done
        .import         _tgi_setcolor
        .export         _tgi_init

_tgi_init:
        pha                             ; Save mode
        jsr     _tgi_done               ; Switch off graphics if needed
        pla
        sta     _tgi_mode               ; Remember the mode
        jsr     tgi_init                ; Go into graphics mode
        jsr     tgi_fetch_error         ; Get the error code
        bne     @L1                     ; Jump on error
        ldx     _tgi_colorcount
        dex
        txa
        jmp     _tgi_setcolor           ; tgi_setcolor (tgi_getmaxcolor ());

@L1:    lda     #$00
        sta     _tgi_mode               ; Clear the mode if init was not successful
        rts

