;
; Ullrich von Bassewitz, 2002-12-03
;
; unsigned char __fastcall__ get_tv (void);
; /* Return the video mode the machine is using */
;

        .export         _get_tv

        .include        "c128.inc"


;--------------------------------------------------------------------------
; _get_tv

.proc   _get_tv

        ldx     #$01            ; Assume PAL
        lda     PALFLAG
        bne     pal
        dex                     ; NTSC
pal:    txa
        ldx     #0
        rts

.endproc


