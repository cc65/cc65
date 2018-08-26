;
; Peter Ferrie, 21.11.2014
;
; off_t __fastcall__ lseek(int fd, off_t offset, int whence);
;

        .export         _lseek
        .import         popax, popptr1
        .macpack        cpu

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"
        .include        "filedes.inc"

_lseek:
        ; Save whence
        sta     tmp1
        stx     tmp2

        ; Get and save offset
        jsr     popptr1
        jsr     popax
        sta     ptr2
        stx     ptr2+1

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
        lda     #$00
        adc     ptr2+1
        bne     einval          ; less than 0 or greater than 2^24 - 1

        ; Set file pointer
        lda     #SET_MARK_CALL
        ldx     #MARK_COUNT
        jsr     callmli
        bcs     oserr

        ; Need to return the position in EAX
.if (.cpu .bitand ::CPU_ISET_65SC02)
        stz     sreg+1
.else
        lda     #$00
        sta     sreg+1
.endif
        lda     mliparam + MLI::MARK::POSITION+2
        sta     sreg
        ldx     mliparam + MLI::MARK::POSITION+1
        lda     mliparam + MLI::MARK::POSITION

        rts

        ; Load errno code
einval: lda     #EINVAL

        ; Set __errno
errno:  jsr     __directerrno   ; leaves -1 in AX
        stx     sreg            ; extend return value to 32 bits
        stx     sreg+1
        rts

        ; Set __oserror
oserr:  jsr     __mappederrno   ; leaves -1 in AX
        stx     sreg            ; extend return value to 32 bits
        stx     sreg+1
        rts
