;
; Oliver Schmidt, 12.01.2005
;
; int __fastcall__ write (int fd, const void* buf, unsigned count);
;

        .export         _write
        .import         rwprolog, rwcommon, rwepilog
        .import         COUT
        .ifndef __APPLE2ENH__
        .import         uppercasemask
        .endif

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "mli.inc"
        .include        "filedes.inc"

_write:
        ; Get parameters
        jsr     rwprolog
        bcs     errno
        tax                     ; Save fd

        ; Check for write access
        lda     fdtab + FD::FLAGS,y
        and     #O_WRONLY
        beq     einval

        ; Check for device
        txa                     ; Restore fd
        bmi     device

        ; Check for append flag
        lda     fdtab + FD::FLAGS,y
        and     #O_APPEND
        beq     write

        ; Set fd
        stx     mliparam + MLI::EOF::REF_NUM

        ; Get file size
        lda     #GET_EOF_CALL
        ldx     #EOF_COUNT
        jsr     callmli
        bcs     oserr

        ; REF_NUM already set
        .assert MLI::MARK::REF_NUM = MLI::EOF::REF_NUM, error

        ; POSITION already set
        .assert MLI::MARK::POSITION = MLI::EOF::EOF, error

        ; Set file pointer
        lda     #SET_MARK_CALL
        ldx     #MARK_COUNT
        jsr     callmli
        bcs     oserr

        ; Do write
write:  lda     fdtab + FD::REF_NUM,y
        ldy     #WRITE_CALL
        jmp     rwcommon

        ; Save count for epilog
device: ldx     ptr2
        lda     ptr2+1
        stx     mliparam + MLI::RW::TRANS_COUNT
        sta     mliparam + MLI::RW::TRANS_COUNT+1

        ; Check for zero count
        ora     ptr2
        beq     done

        ; Get char from buf
        ldy     #$00
next:   lda     (ptr1),y

        ; Replace '\n' with '\r'
        cmp     #$0A
        bne     :+
        lda     #$0D

        ; Set hi bit and write to device
:       ora     #$80
        .ifndef __APPLE2ENH__
        cmp     #$E0            ; Test for lowercase
        bcc     output
        and     uppercasemask
        .endif
output: jsr     COUT            ; Preserves X and Y

        ; Increment pointer
        iny
        bne     :+
        inc     ptr1+1

        ; Decrement count
:       dex
        bne     next
        dec     ptr2+1
        bpl     next

        ; Return success
done:   lda     #$00
        jmp     rwepilog

        ; Load errno code
einval: lda     #EINVAL

        ; Set __errno
errno:  jmp     ___directerrno

        ; Set ___oserror
oserr:  jmp     ___mappederrno

