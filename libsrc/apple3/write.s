;
; Oliver Schmidt, 12.01.2005
; Robert Justice, 2026
;
; int __fastcall__ write (int fd, const void* buf, unsigned count);
;

        .export         _write
        .import         rwprolog, rwcommon, writeepilog
        .import         _cputc

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "sos.inc"
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

        ; Check for not device
        txa                     ; Restore fd
        cmp     #$80
        beq     device

        ; Check for append flag
        lda     fdtab + FD::FLAGS,y
        and     #O_APPEND
        beq     write

        ; Set fd
        stx     sosparam + SOS::GET_EOF::REF_NUM

        ; Get file size
        lda     #GET_EOF_CALL
        ldx     #GET_EOF_COUNT
        jsr     callsos
        bne     oserr

        ; REF_NUM already set
        .assert SOS::GET_MARK::REF_NUM = SOS::GET_EOF::REF_NUM, error

        ; POSITION already set
        .assert SOS::GET_MARK::POSITION = SOS::GET_EOF::EOF, error

        ; Set file pointer
        lda     #SET_MARK_CALL
        ldx     #SET_MARK_COUNT
        jsr     callsos
        bne     oserr

        ; Do write
write:  lda     fdtab + FD::REF_NUM,y
write2: ldy     #WRITE_CALL
        jmp     rwcommon


        ; Save count for epilog
device: ldx     ptr2
        lda     ptr2+1
        stx     sosparam + SOS::RW::TRANS_COUNT
        sta     sosparam + SOS::RW::TRANS_COUNT+1

        ; Check for zero count
        ora     ptr2
        beq     done

        ; Get char from buf
        ldy     #$00
next:   tya                     ; save Y and X
        pha
        txa
        pha
        lda     (ptr1),y
        cmp     #$0A            ; test for \n = line feed
        bne     :+
        jsr     _cputc
        lda     #$0D            ; send carriage return
:       jsr     _cputc
        pla
        tax
        pla
        tay

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
        jmp     writeepilog


        ; Load errno code
einval: lda     #EINVAL

        ; Set __errno
errno:  jmp     ___directerrno

        ; Set ___oserror
oserr:  jmp     ___mappederrno

