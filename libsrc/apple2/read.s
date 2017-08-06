;
; Oliver Schmidt, 12.01.2005
;
; int __fastcall__ read (int fd, void* buf, unsigned count);
;

        .constructor    initprompt
        .export         _read
        .import         rwprolog, rwcommon
        .import         RDKEY, COUT

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "mli.inc"
        .include        "filedes.inc"
        .include        "apple2.inc"

        .segment        "ONCE"

initprompt:
        ; Set prompt <> ']' to let DOS 3.3 know that we're
        ; not in Applesoft immediate mode and thus keep it
        ; from scanning our device I/O for DOS commands.
        lda     #$80            ; Same value used at $D52C
        sta     PROMPT
        rts

        .code

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
        bmi     device

        ; Do read
        ldy     #READ_CALL
        jmp     rwcommon

        ; Device succeeds always
device: lda     #$00
        sta     __oserror

        ; Set counter to zero
        sta     ptr3
        sta     ptr3+1

        ; Check for zero count
        lda     ptr2
        ora     ptr2+1
        beq     check

        ; Read from device and echo to device
next:   jsr     RDKEY
        jsr     COUT

        ; Clear hi bit and check for '\r'
        and     #$7F
        cmp     #$0D
        bne     :+

        ; Replace with '\n' and set count to zero
        lda     #$0A
        ldy     #$00
        sty     ptr2
        sty     ptr2+1

        ; Put char into buf
:       ldy     #$00
        sta     (ptr1),y

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
errno:  jmp     __directerrno
