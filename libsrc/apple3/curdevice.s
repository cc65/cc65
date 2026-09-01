;
; Oliver Schmidt, 2012-10-21
; Robert Justice, 2026
;
; unsigned char getcurrentdevice (void);
;

        .export         _getcurrentdevice, currdev

        .include        "sos.inc"

_getcurrentdevice:

        lda     currdev
        bne     :+           ; Have we initialsed currdev yet?

        lda     #1           ; need to fix this, maybe can find the boot device
:
        ldx     #>$0000
        rts


        .data

currdev:
        .byte   0
