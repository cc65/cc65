;
; Ullrich von Bassewitz, 17.06.1998
;
; unsigned char cursor (unsigned char onoff);
;

        .export         _cursor
        .import         cursor


.proc   _cursor

        tay                     ; onoff into Y
        ldx     #0              ; High byte of result
        lda     cursor          ; Get old value
        sty     cursor          ; Set new value
        rts

.endproc

