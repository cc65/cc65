;
; Christian Groessler, May 2002
;
; this file provides the lseek() function
;
; off_t __fastcall__ lseek(int fd, off_t offset, int whence);
;


        .export         _lseek
        .import         incsp6,__oserror
        .import         __inviocb,ldax0sp,ldaxysp,fdtoiocb
        .import         __dos_type
        .import         fd_table
        .importzp       sreg,ptr1,ptr2,ptr3,ptr4
        .importzp       tmp1,tmp2,tmp3
        .include        "atari.inc"
        .include        "errno.inc"
        .include        "fd.inc"

; seeking not supported, return -1 and ENOSYS errno value
no_supp:jsr     incsp6
        lda     #<ENOSYS
        jsr     __directerrno   ; returns with $FFFF in AX
        sta     sreg
        sta     sreg+1
        rts

parmerr:
iocberr:jsr     incsp6
        ldx     #255
        stx     sreg
        stx     sreg+1
        jmp     __inviocb


; lseek() entry point

.proc   _lseek

        cpx     #0              ; sanity check whence parameter
        bne     parmerr
        cmp     #3              ; valid values are 0..2
        bcs     parmerr
        sta     tmp1            ; remember whence

        ldy     #5
        jsr     ldaxysp         ; get fd
        jsr     fdtoiocb        ; convert to iocb in A, fd_table index in X
        bmi     iocberr
        sta     tmp3            ; remember iocb

        jsr     chk_supp        ; check, whether seeking is supported by DOS/Disk
        bcc     no_supp

        ldx     tmp1            ; whence
        beq     cur
        dex
        beq     end

; SEEK_SET
        dex
        stx     ptr3
        stx     ptr3+1
        stx     ptr4
        stx     ptr4+1
        beq     cont

; SEEK_CUR
cur:    ldx     tmp3
        lda     #NOTE
        sta     ICCOM,x
        jsr     CIOV            ; read it
        bmi     xxerr
l01:    lda     ICAX3,x         ; low byte of position
        sta     ptr3
        lda     ICAX4,x         ; med byte of position
        sta     ptr3+1
        lda     ICAX5,x         ; high byte of position
        sta     ptr4
        lda     #0
        sta     ptr4+1
        beq     cont

; SEEK_END
end:    ldx     tmp3
        lda     #GETFL
        sta     ICCOM,x
        jsr     CIOV
        bpl     l01

; error returned from CIO
xxerr:  tya
        pha
        jsr     incsp6
        pla
        jsr     __mappederrno   ; returns with $FFFF in AX
        sta     sreg
        sta     sreg+1
        rts

; check for offset 0, SEEK_CUR (get current position)
cont:   ldy     #3
        jsr     ldaxysp         ; get upper word
        sta     ptr1
        stx     ptr1+1
        jsr     ldax0sp         ; get lower word
        stx     tmp2
        ora     tmp2
        ora     ptr1
        ora     ptr1+1
        bne     seek
        lda     tmp1            ; whence (0 = SEEK_CUR)
        bne     seek

; offset 0, SEEK_CUR: return current fp
ret:    jsr     incsp6

        lda     ptr4+1
        sta     sreg+1
        lda     ptr4
        sta     sreg
        ldx     ptr3+1
        lda     ptr3

.if 0
        ; return exactly the position DOS has
        ldx     tmp3
        lda     #NOTE
        sta     ICCOM,x
        jsr     CIOV            ; read it
        bmi     xxerr
        lda     #0
        sta     sreg+1
        lda     ICAX5,x         ; high byte of position
        sta     sreg
        lda     ICAX3,x         ; low byte of position
        pha
        lda     ICAX4,x         ; med byte of position
        tax
        pla
.endif

        rts

parmerr1: jmp   parmerr

; we have to call POINT
seek:   jsr     ldax0sp         ; get lower word of new offset
        clc
        adc     ptr3
        sta     ptr3
        txa
        adc     ptr3+1
        sta     ptr3+1
        lda     ptr1
        adc     ptr4
        sta     ptr4
        lda     ptr1+1
        adc     ptr4+1
        sta     ptr4+1
        bne     parmerr1        ; resulting value too large

        ldx     tmp3            ; IOCB
        lda     ptr3
        sta     ICAX3,x
        lda     ptr3+1
        sta     ICAX4,x
        lda     ptr4
        sta     ICAX5,x
        lda     #POINT
        sta     ICCOM,x
        jsr     CIOV
        bpl     ret
        bmi     xxerr

.endproc

; check, whether seeking is supported
; tmp3:         iocb
; X:            index into fd_table
;
; On non-SpartaDOS, seeking is not supported.
; We check, whether CIO function 39 (get file size) returns OK.
; If yes, NOTE and POINT work with real file offset.
; If not, NOTE and POINT work with the standard ScNum/Offset values.
; We remember a successful check in fd_table.ft_flag, bit 3.

chk_supp:
        lda     fd_table+ft_flag,x
        and     #8
        bne     supp

; do the test
        lda     __dos_type
        cmp     #SPARTADOS
        bne     ns1
        txa
        pha
        lda     DOS+1           ; get SpartaDOS version
        cmp     #$40
        bcs     supp1           ; SD-X (ver 4.xx) supports seeking on all disks
        ldx     tmp3            ; iocb to use
        lda     #GETFL
        sta     ICCOM,x
        jsr     CIOV
        bmi     notsupp         ; error code ? should be 168 (invalid command)

; seeking is supported on this DOS/Disk combination

supp1:  pla
        tax
        lda     #8
        ora     fd_table+ft_flag,x
        sta     fd_table+ft_flag,x
supp:   sec
        rts

notsupp:pla
ns1:    clc
        rts

