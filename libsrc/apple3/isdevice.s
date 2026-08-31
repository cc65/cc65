;
; Robert Justice, 2026
;

        .export         isdevice, devname

        .include        "zeropage.inc"
        .include        "sos.inc"

isdevice:
        ; Set device
        sta     sosparam + SOS::DINFO::DEV_NUM

        ; devname buffer
        lda     #<devname
        ldx     #>devname
        sta     sosparam + SOS::DINFO::DEV_NAME
        stx     sosparam + SOS::DINFO::DEV_NAME+1

        ; option list needed to get dev type
        ldx     #3
        stx     sosparam + SOS::DINFO::LENGTH

        ; Get device details
        lda     #D_INFO_CALL
        ldx     #D_INFO_COUNT
        jsr     callsos
        bne     :+         ; error, not valid

        ; check if its a block dev
        lda     sosoption + OPTION::DINFO::DEV_TYPE
        bpl     :+         ; char dev, not valid

        lda     #0         ; dev ok
        rts

:       lda     #$FF
        rts


        .bss

devname: 
        .res   16