;
; Ullrich von Bassewitz, 16.11.2002
;
; int write (int fd, const void* buf, unsigned count);
;

        .export         _write
        .constructor    initstdout

        .import         incsp6
        .import         __errno, __oserror
        .importzp       sp, ptr1, ptr2

        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "cbm.inc"
        .include        "filedes.inc"


;--------------------------------------------------------------------------
; initstdout: Open the stdout and stderr file descriptors for the screen.

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


.proc   _write

        ldy     #4
        lda     (sp),y          ; Get fd

; Check if we have a valid handle

        cmp     #MAX_FDS        ; Is it valid?
        bcs     invalidfd       ; Jump if no

; Check if the LFN is valid and the file is open for writing

        adc     #LFN_OFFS       ; Carry is already clear
        tax
        lda     fdtab-LFN_OFFS,x; Get flags for this handle
        and     #LFN_WRITE      ; File open for writing?
        beq     notopen

; Valid lfn. Make it the output file

        jsr     CKOUT
        bcs     error

; Calculate -count-1 for easier looping

        ldy     #0
        lda     (sp),y
        eor     #$FF
        sta     ptr1
        iny
        lda     (sp),y
        eor     #$FF
        sta     ptr1+1

; Get the pointer to the data buffer

        iny
        lda     (sp),y
        sta     ptr2
        iny
        lda     (sp),y
        sta     ptr2+1
        jmp     deccount

; Read the IEEE488 status

loop:   jsr     READST
        cmp     #0
        bne     error5

        ldy     #0
        lda     (ptr2),y
        inc     ptr2
        bne     @L1
        inc     ptr2+1          ; A = *buf++;

@L1:    jsr     BSOUT

; Decrement count

deccount:
        inc     ptr1
        bne     loop
        inc     ptr1+1
        bne     loop

; Wrote all chars, close the output channel

        jsr     CLRCH

; Return the number of chars written

        ldy     #1
        lda     (sp),y
        tax
        dey
        lda     (sp),y
        jmp     incsp6

; Error entry, file descriptor is invalid

invalidfd:
        lda     #EINVAL
        sta     __errno
        lda     #0
        sta     __errno+1
        beq     errout

; Error entry, file is not open

notopen:
        lda     #3              ; File not open
        bne     error

; Error entry, status not ok

error5: lda     #5              ; Device not present
error:  sta     __oserror
errout: lda     #$FF
        tax                     ; Return -1
        jmp     incsp6

.endproc




