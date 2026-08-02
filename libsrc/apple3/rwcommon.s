;
; Oliver Schmidt, 12.01.2005
; Robert Justice, 2026
;

        .export         rwprolog, rwcommon, readepilog, writeepilog
        .import         popax, popptr1

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "sos.inc"
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
        sta     sosparam + SOS::RW::REF_NUM

        ; Set buf and count
        ; buf (ptr1) goes to mliparam + MLI::RW::DATA_BUFFER,
        ; count (ptr2) goes to mliparam + MLI::RW::REQUEST_COUNT
        ; Make sure both are at expected offset so we can copy them
        ; in a small loop.
        .assert ptr2 = ptr1 + 2, error
        .assert SOS::RW::REQUEST_COUNT = SOS::RW::DATA_BUFFER + 2, error

        ldx     #$03
:       lda     ptr1,x
        sta     sosparam + SOS::RW::DATA_BUFFER,x
        dex
        bpl     :-

        ; Call read or write
        tya
        cmp     #WRITE_CALL
        beq     write
;read
        ; zero out trans count before read
        ; SOS does not seem to clear this when eof is reached
        ldy     #0
        sty     sosparam + SOS::RW::TRANS_COUNT
        sty     sosparam + SOS::RW::TRANS_COUNT+1

        ldx     #READ_COUNT
        jsr     callsos
        beq     readepilog
        cmp     #$4C            ; "End of file encountered"
        bne     oserr

readepilog:
        ; Return success
        sta     ___oserror      ; A = 0
        lda     sosparam + SOS::RW::TRANS_COUNT
        ldx     sosparam + SOS::RW::TRANS_COUNT+1
        rts

write:  ldx     #WRITE_COUNT    ; sos write count is different
        jsr     callsos
        beq     writeepilog
        cmp     #$4C            ; "End of file encountered"
        bne     oserr

writeepilog:
        ; Return success
        sta     ___oserror      ; A = 0
        lda     sosparam + SOS::RW::REQUEST_COUNT
        ldx     sosparam + SOS::RW::REQUEST_COUNT+1
        rts

        ; Set ___oserror
oserr:  jmp     ___mappederrno
