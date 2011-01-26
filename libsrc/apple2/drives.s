;
; Oliver Schmidt, 2010-05-24
;
; unsigned char drivecount (void);
; unsigned char* drivelist (void);
;

        .export         _drivecount, _drivelist
        .import         __dos_type

        .include        "mli.inc"

_drivecount:
        ldx     #$00

        ; Check for ProDOS 8
        lda     __dos_type
        beq     :+

        ; Number of on-line devices (minus 1)
        ldy     DEVCNT
        iny
        tya
:       rts

_drivelist:
        ldx     #$00

        ; Check for ProDOS 8
        lda     __dos_type
        beq     :+

        ; Up to 14 units may be active
        lda     #<DEVLST
        ldx     #>DEVLST
:       rts
