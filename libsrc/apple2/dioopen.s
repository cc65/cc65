;
; Oliver Schmidt, 24.03.2005
;
; dhandle_t __fastcall__ dio_open (unsigned char device);
;

        .export         _dio_open
        .import         return0, __dos_type, isdevice

        .include        "errno.inc"
        .include        "mli.inc"
        .include        "zeropage.inc"

_dio_open:
        ; Check for ProDOS 8
        ldx     __dos_type
        bne     :+
        lda     #$01            ; "Bad system call number"
        bne     oserr           ; Branch always

        ; Convert from 00DDDSSS format to DSSS00DD format
:		tax
        clc
        asl
        asl
        asl
        asl
        sta     tmp1
        txa
        lsr
        lsr
        lsr
        lsr
        ora     tmp1

        ; Check for valid device
        tax

        jsr     isdevice
        beq     :+
        lda     #$28            ; "No device connected"

        ; Return oserror
oserr:  sta     __oserror
        jmp     return0

        ; Return success
:       txa

        ldx     #$00
        stx     __oserror
        rts
