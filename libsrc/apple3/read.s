;
; Oliver Schmidt, 12.01.2005
; Robert Justice, 2026
;
; int __fastcall__ read (int fd, void* buf, unsigned count);
;

        .export         _read
        .import         rwprolog, rwcommon
        .import         getchar, _cputc

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "sos.inc"
        .include        "filedes.inc"
        .include        "apple3.inc"


_read:
        ; Get parameters
        jsr     rwprolog
        bcs     errno
        tax                     ; Save fd

        ; Check for read access
        lda     fdtab + FD::FLAGS,y
        and     #O_RDONLY
        beq     einval

        ; Check for device
        txa                     ; Restore fd
        cmp     #$80
        beq     device

        ; Do read
        ldy     #READ_CALL
        jmp     rwcommon

       ; Device succeeds always
device: lda     #$00
        sta     ___oserror

        ; Set counter to zero
        sta     ptr3
        sta     ptr3+1

        ; Check for zero count
        lda     ptr2
        ora     ptr2+1
        beq     check

        ; Turn cursor on
        lda     #CONSOLE_CURSOR_ON
        jsr     _cputc

        ; Read from device
next:   jsr     getchar

        ; We'll need Y=0 in both branches below
        ldy     #$00

        ; Check for '\r'
        cmp     #$0D
        bne     :+
        jsr     _cputc     ; echo \r

        ldy     #$00
        ; Replace with '\n' and set count to zero
        lda     #$0A
        sty     ptr2
        sty     ptr2+1

        ; Put char into buf
:       sta     (ptr1),y

        pha                 ; echo here after cr->lf conversion
        jsr     _cputc
        pla

        ; Increment pointer
        inc     ptr1
        bne     :+
        inc     ptr1+1

        ; Increment counter
:       inc     ptr3
        bne     check
        inc     ptr3+1

        ; Check for counter less than count
check:  lda     ptr3
        cmp     ptr2
        bcc     next
        ldx     ptr3+1
        cpx     ptr2+1
        bcc     next

        ; Return success, AX already set
        rts

        ; Load errno code
einval: lda     #EINVAL

        ; Set __errno
errno:  jmp     ___directerrno
