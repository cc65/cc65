;
; unsigned char cursor (unsigned char onoff);
;

        .include "cbm_kernal.inc"

        .export         _cursor
        .import         cursor

.proc   _cursor
        pha
        ; A != 0 to enable, 0 to disable
        cmp     #0
        beq     disable ; C = 1
        clc
disable:
        ; C = 0 to enable, 1 to disable
        jsr     CURSOR

        ply                     ; onoff into Y
        ldx     #0              ; High byte of result
        lda     cursor          ; Get old value
        sty     cursor          ; Set new value
        rts
.endproc

