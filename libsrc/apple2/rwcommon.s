;
; Oliver Schmidt, 12.01.2005
;

        .export         rwprolog, rwcommon, rwepilog
        .import         popax, popptr1

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "mli.inc"
        .include        "filedes.inc"

rwprolog:
        ; Save count
        sta     ptr2
        stx     ptr2+1

        ; Get and save buf
        jsr     popptr1

        ; Get and process fd
        jsr     popax
        jmp     getfd           ; Returns A, Y and C

rwcommon:
        ; Set fd
        sta     mliparam + MLI::RW::REF_NUM

        ; Set buf and count
        ; buf (ptr1) goes to mliparam + MLI::RW::DATA_BUFFER,
        ; count (ptr2) goes to mliparam + MLI::RW::REQUEST_COUNT
        ; Make sure both are at expected offset so we can copy them
        ; in a small loop.
        .assert ptr2 = ptr1 + 2, error
        .assert MLI::RW::REQUEST_COUNT = MLI::RW::DATA_BUFFER + 2, error

        ldx     #$03
:       lda     ptr1,x
        sta     mliparam + MLI::RW::DATA_BUFFER,x
        dex
        bpl     :-

        ; Call read or write
        tya
        ldx     #RW_COUNT
        jsr     callmli
        bcc     rwepilog
        cmp     #$4C            ; "End of file encountered"
        bne     oserr

rwepilog:
        ; Return success
        sta     ___oserror      ; A = 0
        lda     mliparam + MLI::RW::TRANS_COUNT
        ldx     mliparam + MLI::RW::TRANS_COUNT+1
        rts

        ; Set ___oserror
oserr:  jmp     ___mappederrno
