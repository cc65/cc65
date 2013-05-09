;
; Oliver Schmidt, 12.01.2005
;

        .export         rwprolog, rwcommon, rwepilog
        .import         popax

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
        jsr     popax
        sta     ptr1
        stx     ptr1+1

        ; Get and process fd
        jsr     popax
        jmp     getfd           ; Returns A, Y and C

rwcommon:
        ; Set fd
        sta     mliparam + MLI::RW::REF_NUM

        ; Set buf
        lda     ptr1
        ldx     ptr1+1
        sta     mliparam + MLI::RW::DATA_BUFFER
        stx     mliparam + MLI::RW::DATA_BUFFER+1

        ; Set count
        lda     ptr2
        ldx     ptr2+1
        sta     mliparam + MLI::RW::REQUEST_COUNT
        stx     mliparam + MLI::RW::REQUEST_COUNT+1

        ; Call read or write
        tya
        ldx     #RW_COUNT
        jsr     callmli
        bcc     rwepilog
        cmp     #$4C            ; "End of file encountered"
        bne     oserr

rwepilog:
        ; Return success
        sta     __oserror       ; A = 0
        lda     mliparam + MLI::RW::TRANS_COUNT
        ldx     mliparam + MLI::RW::TRANS_COUNT+1
        rts

        ; Set __oserror
oserr:  jmp     __mappederrno
