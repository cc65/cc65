;
; Ullrich von Bassewitz, 16.11.2002
;
; int read (int fd, void* buf, unsigned count);
;

        .export         _read
        .constructor    initstdin
           
        .import         rwcommon
        .import         popax
        .import         __errno, __oserror
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2, tmp3

        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "cbm.inc"
        .include        "filedes.inc"


;--------------------------------------------------------------------------
; initstdin: Open the stdin file descriptors for the keyboard

.proc   initstdin

        lda     #LFN_READ
        sta     fdtab+STDIN_FILENO
        lda     #STDIN_FILENO + LFN_OFFS
        ldx     #CBMDEV_KBD
        stx     unittab+STDIN_FILENO
        ldy     #$FF
        jsr     SETLFS
        jmp     OPEN            ; Will always succeed

.endproc

;--------------------------------------------------------------------------
; _read


.proc   _read

        jsr     rwcommon        ; Pop params, check handle
        bcs     invalidfd       ; Branch if handle not ok

; Check if the LFN is valid and the file is open for writing

        adc     #LFN_OFFS       ; Carry is already clear
        tax
        lda     fdtab-LFN_OFFS,x; Get flags for this handle
        and     #LFN_READ       ; File open for writing?
        beq     notopen

; Check the EOF flag. If it is set, don't read anything

        lda     fdtab-LFN_OFFS,x; Get flags for this handle
        bmi     eof

; Valid lfn. Make it the input file

        jsr     CHKIN
        bcs     error

; Go looping...

        bcc     @L3             ; Branch always

; Read the next byte

@L0:    jsr     BASIN
        sta     tmp1            ; Save the input byte

        jsr     READST          ; Read the IEEE status
        sta     tmp3            ; Save it
        and     #%10111111      ; Check anything but the EOI bit
        bne     error5          ; Assume device not present

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

; Return the number of chars read

eof:    lda     ptr3
        ldx     ptr3+1
        rts

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
        rts

.endproc




