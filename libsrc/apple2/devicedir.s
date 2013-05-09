;
; Oliver Schmidt, 2010-05-24
;
; char* __fastcall__ getdevicedir (unsigned char device, char* buf, size_t size);
;

        .export         _getdevicedir
        .import         popax, popa

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"

_getdevicedir:
        ; Save size
        sta     ptr2
        stx     ptr2+1

        ; Save buf
        jsr     popax
        sta     ptr1
        stx     ptr1+1

        ; Set buf
        sta     mliparam + MLI::ON_LINE::DATA_BUFFER
        stx     mliparam + MLI::ON_LINE::DATA_BUFFER+1

        ; Set device
        jsr     popa
        asl
        asl
        asl
        asl
        sta     mliparam + MLI::ON_LINE::UNIT_NUM

        ; Check for valid slot
        and     #$70
        beq     erange

        ; Check for sufficient buf size
        lda     ptr2+1
        bne     :++             ; Buf >= 256
        lda     ptr2
        cmp     #17
        bcs     :++             ; Buf >= 17

        ; Handle errors
erange: lda     #<ERANGE
        jsr     __directerrno
        bne     :+              ; Branch always
oserr:  jsr     __mappederrno
:       lda     #$00            ; Return NULL
        tax
        rts

        ; Get volume name
:       lda     #ON_LINE_CALL
        ldx     #ON_LINE_COUNT
        jsr     callmli
        bcs     oserr

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
        sta     __oserror       ; Clear _oserror

        ; Success, return buf
        lda     ptr1
        ldx     ptr1+1
        rts
