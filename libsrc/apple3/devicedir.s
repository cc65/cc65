;
; Oliver Schmidt, 2010-05-24
; Robert Justice, 2026
;
; char* __fastcall__ getdevicedir (unsigned char device, char* buf, size_t size);
;

        .export         _getdevicedir
        .import         popptr1, popa, devname

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "sos.inc"

_getdevicedir:
        ; Save size
        sta     ptr2
        stx     ptr2+1

        ; Save buf
        jsr     popptr1

        ; Set buffer for devname
        lda     #<devname
        ldx     #>devname
        sta     sosparam + SOS::DINFO::DEV_NAME
        stx     sosparam + SOS::DINFO::DEV_NAME+1

        ; no option list needed
        ldx     #0
        stx     sosparam + SOS::DINFO::LENGTH

        ; Set device
        jsr     popa
        sta     sosparam + SOS::DINFO::DEV_NUM

        ; Check for valid dev num
        cmp     #$19
        bcs     erange

        ; Check for sufficient buf size
        lda     ptr2+1
        bne     :++             ; Buf >= 256
        lda     ptr2
        cmp     #17
        bcs     :++             ; Buf >= 17

        ; Handle errors
erange: lda     #<ERANGE
        jsr     ___directerrno
        bne     :+              ; Branch always
oserr:  jsr     ___mappederrno
:       lda     #$00            ; Return NULL
        tax
        rts

        ; Get device name
:       lda     #D_INFO_CALL
        ldx     #D_INFO_COUNT
        jsr     callsos
        bne     oserr

        ; Copy over DEV_NAME ptr
        lda     sosparam + SOS::DINFO::DEV_NAME
        ldx     sosparam + SOS::DINFO::DEV_NAME+1
        sta     sosparam + SOS::VOLUME::DEV_NAME
        stx     sosparam + SOS::VOLUME::DEV_NAME+1

        ; Set buf
        lda     ptr1
        ldx     ptr1+1
        sta     sosparam + SOS::VOLUME::VOL_NAME
        stx     sosparam + SOS::VOLUME::VOL_NAME+1

        ; Get volume name
        lda     #VOLUME_CALL
        ldx     #VOLUME_COUNT
        jsr     callsos
        bne     oserr

        ; Get volume name length
        ldy     #$00
        lda     (ptr1),y
        and     #15             ; Max volume name length
        sta     tmp1

        ; Add leading slash
        lda     #'/'
        sta     (ptr1),y

        ; Add terminating zero
        ldy     tmp1
        iny
        lda     #$00
        sta     (ptr1),y
        sta     ___oserror      ; Clear __oserror

        ; Success, return buf
        lda     ptr1
        ldx     ptr1+1
        rts
