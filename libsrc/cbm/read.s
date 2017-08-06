;
; 2002-11-16, Ullrich von Bassewitz
; 2013-12-23, Greg King
;
; int read (int fd, void* buf, unsigned count);
;

        .export         _read
        .constructor    initstdin

        .import         SETLFS, OPEN, CHKIN, BASIN, CLRCH, BSOUT, READST
        .import         rwcommon
        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2, tmp3

        .include        "cbm.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "filedes.inc"


;--------------------------------------------------------------------------
; initstdin: Open the stdin file descriptors for the keyboard

.segment        "ONCE"

.proc   initstdin

        lda     #STDIN_FILENO + LFN_OFFS
        ldx     #CBMDEV_KBD
        ldy     #$FF
        jsr     SETLFS
        jmp     OPEN            ; Will always succeed

.endproc

;--------------------------------------------------------------------------
; _read

.code

.proc   _read

        jsr     rwcommon        ; Pop params, check handle
        bcs     invalidfd       ; Invalid handle

; Check if the LFN is valid and the file is open for writing

        adc     #LFN_OFFS       ; Carry is already clear
        tax
        lda     fdtab-LFN_OFFS,x; Get flags for this handle
        tay
        and     #LFN_READ       ; File open for writing?
        beq     invalidfd

; Check the EOF flag. If it is set, don't read anything

        tya                     ; Get flags again
        bmi     eof

; Remember the device number.

        ldy     unittab-LFN_OFFS,x
        sty     unit

; Valid lfn. Make it the input file

        jsr     CHKIN
        bcc     @L3             ; Branch if ok
        jmp     __mappederrno   ; Store into __oserror, map to errno, return -1

; Read the next byte

@L0:    jsr     BASIN
        sta     tmp1            ; Save the input byte
        ldx     unit
        bne     @L0_1           ; Not keyboard/screen-editor
        cmp     #$0D            ; Is it a Carriage Return?
        bne     @L0_1
        jsr     BSOUT           ; Yes, echo it (because editor didn't)

@L0_1:  jsr     READST          ; Read the IEEE status
        sta     tmp3            ; Save it
        and     #%10111111      ; Check anything but the EOI bit
        bne     devnotpresent   ; Assume device not present

; Store the byte just read

        ldy     #0
        lda     tmp1
        sta     (ptr2),y
        inc     ptr2
        bne     @L1
        inc     ptr2+1          ; *buf++ = A;

; Increment the byte count

@L1:    inc     ptr3
        bne     @L2
        inc     ptr3+1

; Get the status again and check the EOI bit

@L2:    lda     tmp3
        and     #%01000000      ; Check for EOI
        bne     @L4             ; Jump if end of file reached

; Decrement the count

@L3:    inc     ptr1
        bne     @L0
        inc     ptr1+1
        bne     @L0
        beq     done            ; Branch always

; Set the EOI flag and bail out

@L4:    ldx     tmp2            ; Get the handle
        lda     #LFN_EOF
        ora     fdtab,x
        sta     fdtab,x

; Read done, close the input channel

done:   jsr     CLRCH

; Clear _oserror and return the number of chars read

eof:    lda     #0
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


;--------------------------------------------------------------------------

.bss

unit:   .res    1
