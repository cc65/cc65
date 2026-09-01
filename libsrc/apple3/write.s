;
; Oliver Schmidt, 12.01.2005
; Robert Justice, 2026
;
; int __fastcall__ write (int fd, const void* buf, unsigned count);
;

        .export         _write
        .import         rwprolog, rwcommon, writeepilog
        .import         putcdirect, consref, setstdioscr, consscrflg

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


        ; Load errno code
einval: lda     #EINVAL

        ; Set __errno
errno:  jmp     ___directerrno

        ; Set ___oserror
oserr:  jmp     ___mappederrno


        ; Save request count for return
device: ldx     ptr2
        stx     tmp3
        stx     ptr3
        lda     ptr2+1
        sta     tmp4
        sta     ptr3+1

        ; Set console ref
        ldy     consref
        sty     sosparam + SOS::RW::REF_NUM

        ; Check for zero count
        ora     ptr2
        beq     done

        ; check for zero low byte with non zero high byte
        txa
        bne     :+
        dec     ptr3+1

        ; Copy ptr1 to ptr4 to use for scan pos
:       lda     ptr1
        sta     ptr4
        lda     ptr1+1
        sta     ptr4+1

        ; Check char from buf
        ldy     #$00            ; y = chunk size low byte
        sty     ptr2+1          ; zero chunk size high byte
next:   lda     (ptr4),y
        cmp     #$0A            ; test for \n = line feed
        beq     havelf

        ; Increment pointer
        iny
        bne     :+
        inc     ptr4+1
        inc     ptr2+1

        ; Decrement count
:       dex
        bne     next
        lda     ptr3+1
        beq     outdone
        dec     ptr3+1
        ldx     #$00
        beq     next

        ; Output when we get a lf
havelf: iny
        jsr     output
        lda     #$0D            ; send carriage return
        jsr     putcdirect

        ; Update ptr1
        lda     ptr1
        clc
        adc     ptr2
        sta     ptr1
        lda     ptr1+1
        adc     ptr2+1
        sta     ptr1+1

        ; ptr4 = new chunk start
        lda     ptr1
        sta     ptr4
        lda     ptr1+1
        sta     ptr4+1

        ; Restore X, Y
        ldx     ptr3
        ldy     #0
        sty     ptr2+1

        ; Dec count for lf, and exit if done
        dex
        bne     next
        lda     ptr3+1
        beq     done
        dec     ptr3+1
        ldx     #$00
        beq     next

        ; Output whatever is left
outdone:
        jsr     output


        ; Return success
done:   lda     #$00
        sta     ___oserror      ; A = 0
        lda     tmp3            ; original requested bytes
        ldx     tmp4
        rts

        ; output to console
output: sty     ptr2           ; save Y
        stx     ptr3           ; save X

        ldx     #$03
:       lda     ptr1,x
        sta     sosparam + SOS::RW::DATA_BUFFER,x
        dex
        bpl     :-

        bit     consscrflg    ; check if scroll is on
        bne     :+
        jsr     setstdioscr
:
        lda     #WRITE_CALL
        ldx     #WRITE_COUNT
        jsr     callsos
        rts
