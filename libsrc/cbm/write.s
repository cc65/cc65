;
; Ullrich von Bassewitz, 16.11.2002
;
; int write (int fd, const void* buf, unsigned count);
;

        .export         _write
        .constructor    initstdout

        .import         rwcommon
        .importzp       sp, ptr1, ptr2, ptr3

        .include        "cbm.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "filedes.inc"


;--------------------------------------------------------------------------
; initstdout: Open the stdout and stderr file descriptors for the screen.

.segment        "ONCE"

.proc   initstdout

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
@error: jmp     ___mappederrno   ; Store into ___oserror, map to errno, return -1

; Output the next character from the buffer

@L0:    ldy     #0
        lda     (ptr1),y
        inc     ptr1
        bne     @L1
        inc     ptr1+1          ; A = *buf++;
@L1:    jsr     BSOUT

; Check the status

        pha
        jsr     READST
        lsr     a               ; Bit zero is write timeout
        bne     devnotpresent2
        pla
        bcs     @L3

; Count characters written

        inc     ptr3
        bne     @L2
        inc     ptr3+1

; Decrement count

@L2:    dec     ptr2
        bne     @L0
        dec     ptr2+1
        bne     @L0

; Wrote all chars or disk full. Close the output channel

@L3:    jsr     CLRCH

; Clear __oserror and return the number of chars written

        lda     #0
        sta     ___oserror
        lda     ptr3
        ldx     ptr3+1
        rts

; Error entry: Device not present

devnotpresent2:
        pla
devnotpresent:
        lda     #ENODEV
        .byte   $2C             ; Skip next opcode via BIT <abs>

; Error entry: The given file descriptor is not valid or not open

invalidfd:
        lda     #EBADF
        jmp     ___directerrno   ; Sets _errno, clears __oserror, returns -1

.endproc
