;
; Peter Ferrie, 21.11.2014
; Robert Justice, 2026
;
; off_t __fastcall__ lseek(int fd, off_t offset, int whence);
;

        .export         _lseek
        .import         popax, popptr1

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "sos.inc"
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
        beq     einval

        ; Valid whence values are 0..2
        ldx     tmp2
        bne     einval
        ldx     tmp1
        cpx     #3
        bcs     einval

        ; Set fd
        sta     sosparam + SOS::GET_MARK::REF_NUM

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
        .assert GET_MARK_COUNT = GET_EOF_COUNT, error
        ldx     #GET_MARK_COUNT
        jsr     callsos
        bne     oserr
        lda     sosparam + SOS::GET_MARK::POSITION
        ldx     sosparam + SOS::GET_MARK::POSITION+1
        ldy     sosparam + SOS::GET_MARK::POSITION+2

seek_common:
        clc
        adc     ptr1
        sta     sosparam + SOS::SET_MARK::POSITION
        txa
        adc     ptr1+1
        sta     sosparam + SOS::SET_MARK::POSITION+1
        tya
        adc     ptr2
        sta     sosparam + SOS::SET_MARK::POSITION+2
        lda     #$00
        adc     ptr2+1
        bne     einval          ; less than 0 or greater than 2^24 - 1

        lda     #0
        sta     sosparam + SOS::SET_MARK::POSITION+3
        sta     sosparam + SOS::SET_MARK::BASE


        ; Set file pointer
        lda     #SET_MARK_CALL
        ldx     #SET_MARK_COUNT
        jsr     callsos
        bne     oserr

        lda     #$00
        sta     sreg+1
        lda     sosparam + SOS::SET_MARK::POSITION+2
        sta     sreg
        ldx     sosparam + SOS::SET_MARK::POSITION+1
        lda     sosparam + SOS::SET_MARK::POSITION

        rts

        ; Load errno code
einval: lda     #EINVAL

        ; Set __errno
errno:  jsr     ___directerrno  ; leaves -1 in AX
        stx     sreg            ; extend return value to 32 bits
        stx     sreg+1
        rts

        ; Set ___oserror
oserr:  jsr     ___mappederrno  ; leaves -1 in AX
        stx     sreg            ; extend return value to 32 bits
        stx     sreg+1
        rts
