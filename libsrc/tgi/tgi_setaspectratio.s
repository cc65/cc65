;
; Ullrich von Bassewitz, 2011-05-01
;
; void __fastcall__ tgi_setaspectratio (unsigned aspectratio);
; /* Set a new aspect ratio for the loaded driver. The aspect ratio is an
; ** 8.8 fixed point value.
; */
;

        .include        "tgi-kernel.inc"

.proc   _tgi_setaspectratio

        sta     _tgi_aspectratio
        stx     _tgi_aspectratio+1
        rts

.endproc

