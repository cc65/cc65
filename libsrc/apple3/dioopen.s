;
; Oliver Schmidt, 24.03.2005
; Robert Justice, 2026
;
; dhandle_t __fastcall__ dio_open (unsigned char device);
;

        .export         _dio_open
        .import         return0, isdevice, currdev

        .include        "errno.inc"
        .include        "sos.inc"

_dio_open:
        ; Check for valid device
        pha
        jsr     isdevice
        beq     :+
        lda     #$28            ; "No device connected"

        ; Return oserror
oserr:  sta     ___oserror
        jmp     return0

        ; Return success
:       pla
        sta     currdev
        ldx     #$00
        stx     ___oserror
        rts
