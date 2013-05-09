;
; Ullrich von Bassewitz, 2002-12-03
;
; unsigned char __fastcall__ get_tv (void);
; /* Return the video mode the machine is using */
;

        .include        "plus4.inc"
        .include        "get_tv.inc"


;--------------------------------------------------------------------------
; _get_tv

.proc   _get_tv

        ldx     #TV::PAL        ; Assume PAL
        bit     TED_MULTI1      ; Test bit 6
        bvc     pal
        dex                     ; NTSC
pal:    txa
        ldx     #0
        rts

.endproc


