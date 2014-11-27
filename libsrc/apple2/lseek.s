;
; Peter Ferrie, 21.11.2014
;
; off_t __fastcall__ lseek(int fd, off_t offset, int whence);
;

        .export         _lseek
        .import         popax

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"
        .include        "filedes.inc"

_lseek:
        ; Save whence
        sta     tmp1
        stx     tmp2

        ; Get and save offset
        jsr     popax
        sta     ptr1
        stx     ptr1+1
        jsr     popax
        sta     ptr2

        ; Get and process fd
        jsr     popax
        jsr     getfd           ; Returns A, Y and C
        bcs     errno

        ; Check for device
        cmp     #$80
        bcs     einval

        ; Valid whence values are 0..2
        ldx     tmp2
        bne     einval
        ldx     tmp1
        cpx     #3
        bcs     einval

        ; Set fd
        sta     mliparam + MLI::MARK::REF_NUM

        txa
        beq     cur
        lda     #GET_EOF_CALL
        dex
        beq     end

; SEEK_SET
        dex
        txa
        tay
        beq     seek_common

; SEEK_CUR
cur:
        lda     #GET_MARK_CALL

; SEEK_END
end:
        ; MARK_COUNT must == EOF_COUNT, otherwise unexpected behaviour
        .assert MARK_COUNT = EOF_COUNT, error
        ldx     #MARK_COUNT
        jsr     callmli
        bcs     oserr
        lda     mliparam + MLI::MARK::POSITION
        ldx     mliparam + MLI::MARK::POSITION+1
        ldy     mliparam + MLI::MARK::POSITION+2

seek_common:
        adc     ptr1
        sta     mliparam + MLI::MARK::POSITION
        txa
        adc     ptr1+1
        sta     mliparam + MLI::MARK::POSITION+1
        tya
        adc     ptr2
        sta     mliparam + MLI::MARK::POSITION+2

        ; Set file pointer
        lda     #SET_MARK_CALL
        ldx     #MARK_COUNT
        jsr     callmli
        bcs     oserr

        rts

        ; Load errno code
einval: lda     #EINVAL

        ; Set __errno
errno:  jmp     __directerrno

        ; Set __oserror
oserr:  jmp     __mappederrno
