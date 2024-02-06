;
; Oliver Schmidt, 2012-09-04
; Christian Groessler, 2017-12-28
;
; unsigned char getfirstdevice (void);
; unsigned char __fastcall__ getnextdevice (unsigned char device);
;

        .include        "atari.inc"
        .export         _getfirstdevice
        .export         _getnextdevice

MAX_DIO_DEVICES =       8

;------------------------------------------------------------------------------
; _getfirstdevice

_getfirstdevice:
        lda     #$FF
        ; Fall through

;------------------------------------------------------------------------------
; _getnextdevice

_getnextdevice:
        tax
next:   inx
        cpx     #MAX_DIO_DEVICES
        beq     none

        jsr     check_device
        bmi     next

done:   txa
        ldx     #$00
        rts

none:   ldx     #255            ; INVALID_DEVICE (see include/device.h)
        bne     done            ; jump always

;------------------------------------------------------------------------------
; check_device - checks if a disk device is present
; input:        X  - device id (0 = D1, 1 = D2, ...)
; output:       NF - 0/1 for detected/not detected
; X register preserved

check_device:
        txa
        pha
        lda     #SIO_STAT
        sta     DCOMND          ; set command into DCB
        lda     #%01000000      ; direction value, "receive data"
        sta     DSTATS          ; set data flow directon
        lda     #15
        sta     DTIMLO          ; value got from DOS source
        lda     #4
        sta     DAUX1           ; set sector #  (dummy: 4)
        sta     DBYTLO          ; # of bytes to transfer
        lda     #0
        sta     DAUX2
        sta     DBYTHI
        lda     #>DVSTAT
        sta     DBUFHI
        lda     #<DVSTAT
        sta     DBUFLO          ; set buffer address into DCB
        lda     #DISKID         ; SIO bus ID of diskette drive
        sta     DDEVIC
        inx
        stx     DUNIT           ; unit number (1-based)

        jsr     SIOV            ; execute SIO command

        pla
        tax
        lda     DSTATS
        rts

        .end
