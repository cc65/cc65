;
; Stefan Haubenthal, 2018-04-10
; Based on code by Mike
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using */
;

        .include        "get_tv.inc"

;--------------------------------------------------------------------------
; _get_tv

.proc   _get_tv

        ; detect the system
        lda     #TV::NTSC
        tax
        ldy     $EDE4           ; VIC init table
        cpy     #5
        beq     @L0
        lda     #TV::PAL
@L0:    rts                     ; system detected: 0 for NTSC, 1 for PAL

.endproc
