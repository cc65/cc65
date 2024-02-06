;
; Oliver Schmidt, 2012-10-21
;
; unsigned char getcurrentdevice (void);
;

        .export         _getcurrentdevice
        .import         __dos_type

        .include        "mli.inc"

_getcurrentdevice:

        ; Use unit number of most recent accessed device
        lda     DEVNUM
        lsr
        lsr
        lsr
        lsr

        ; Check for ProDOS 8
        ldx     __dos_type
        bne     :+
        lda     #$FF            ; INVALID_DEVICE

:       ldx     #$00
        rts
