;
; Robert Justice, 2026
;
; char* __fastcall__ getdevicename (unsigned char device, char* buf, size_t size);
;

        .export         _getdevicename
        .import         popptr1, popa

        .include        "zeropage.inc"
        .include        "sos.inc"

_getdevicename:
        ; Check size
        sta     ptr2
        stx     ptr2+1

        ; Save buf
        jsr     popptr1

        ; Set buf
        sta     sosparam + SOS::DINFO::DEV_NAME
        ldx     ptr1+1
        stx     sosparam + SOS::DINFO::DEV_NAME+1

        ; Set device
        jsr     popa
        sta     sosparam + SOS::DINFO::DEV_NUM

        ; Get device name
        lda     #D_INFO_CALL
        ldx     #D_INFO_COUNT
        jsr     callsos
        ;bne     oserr

        ; Get device name length
        ldy     #$00
        lda     (ptr1),y
        and     #15             ; Max device name length
        tay

        ; Add terminating zero
        iny
        lda     #$00
        sta     (ptr1),y

        ; inc ptr1 to skip length byte
        inc     ptr1
        bne     :+
        inc     ptr1+1
:
        lda     ptr1
        ldx     ptr1+1
        rts
