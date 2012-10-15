;
; Oliver Schmidt, 2012-09-04
;
; unsigned char getfirstdevice (void);
; unsigned char __fastcall__ getnextdevice (unsigned char device);
;

        .export         _getfirstdevice
        .export         _getnextdevice
        .import         __dos_type, isdevice

        .include        "zeropage.inc"

_getfirstdevice:
        lda     #$FF
        ; Fall through

_getnextdevice:
        tax
next:   inx
        cpx     #$FF
        beq     done

        ; Check for ProDOS 8
        lda     __dos_type
        beq     next

        ; Check for valid device
        jsr     isdevice
        bne     next

done:   txa
        ldx     #$00
        rts
