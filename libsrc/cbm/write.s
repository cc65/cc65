;
; Ullrich von Bassewitz, 16.11.2002
;
; int write (int fd, const void* buf, unsigned count);
;

        .export         _write
        .constructor    initstdout

        .import         SETLFS, OPEN, CKOUT, BSOUT, CLRCH
        .import         rwcommon
        .importzp       sp, ptr1, ptr2, ptr3

        .include        "cbm.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "filedes.inc"


;--------------------------------------------------------------------------
; initstdout: Open the stdout and stderr file descriptors for the screen.

.segment        "INIT"

.proc   initstdout

        lda     #LFN_WRITE
        sta     fdtab+STDOUT_FILENO
        sta     fdtab+STDERR_FILENO
        lda     #CBMDEV_SCREEN
        sta     unittab+STDOUT_FILENO
        sta     unittab+STDERR_FILENO
        lda     #STDOUT_FILENO + LFN_OFFS
        jsr     @L1
        lda     #STDERR_FILENO + LFN_OFFS
@L1:    ldx     #CBMDEV_SCREEN
        ldy     #$FF
        jsr     SETLFS
        jmp     OPEN            ; Will always succeed

.endproc

;--------------------------------------------------------------------------
; _write

.code

.proc   _write

        jsr     rwcommon        ; Pop params, check handle
        bcs     invalidfd       ; Invalid handle

; Check if the LFN is valid and the file is open for writing

        adc     #LFN_OFFS       ; Carry is already clear
        tax
        lda     fdtab-LFN_OFFS,x; Get flags for this handle
        and     #LFN_WRITE      ; File open for writing?
        beq     invalidfd

; Valid lfn. Make it the output file

        jsr     CKOUT
        bcc     @L2
@error: jmp     __mappederrno   ; Store into __oserror, map to errno, return -1

; Output the next character from the buffer

@L0:    ldy     #0
        lda     (ptr2),y
        inc     ptr2
        bne     @L1
        inc     ptr2+1          ; A = *buf++;
@L1:    jsr     BSOUT
        bcs     @error          ; Bail out on errors

; Count characters written

        inc     ptr3
        bne     @L2
        inc     ptr3+1

; Decrement count

@L2:    inc     ptr1
        bne     @L0
        inc     ptr1+1
        bne     @L0

; Wrote all chars, close the output channel

        jsr     CLRCH

; Clear _oserror and return the number of chars written

        lda     #0
        sta     __oserror
        lda     ptr3
        ldx     ptr3+1
        rts

; Error entry: Device not present

devnotpresent:
        lda     #ENODEV
        jmp     __directerrno   ; Sets _errno, clears _oserror, returns -1

; Error entry: The given file descriptor is not valid or not open

invalidfd:
        lda     #EBADF
        jmp     __directerrno   ; Sets _errno, clears _oserror, returns -1

.endproc




