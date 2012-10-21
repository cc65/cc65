;
; Oliver Schmidt, 2012-10-21
;
; unsigned char getcurrentdevice (void);
;

        .export         _getcurrentdevice
        .import         curunit

;------------------------------------------------------------------------------
; _getcurrentdevice

.proc   _getcurrentdevice

        lda     curunit
        ldx     #$00
        rts

.endproc
