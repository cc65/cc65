;
; Ullrich von Bassewitz, 16.11.2002
;
; int read (int fd, void* buf, unsigned count);
;

        .export         _read
        .constructor    initstdin

        .import         popax
        .import         __errno, __oserror
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2

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

; Retrieve count

        jsr     popax           ; Get count
        eor     #$FF
        sta     ptr1
        txa
        eor     #$FF
        sta     ptr1+1          ; Remember -count-1

; Retrieve buf

        jsr     popax
        sta     ptr2
        stx     ptr2+1

; Retrieve the handle

        jsr     popax

; Check if we have a valid handle

        cpx     #$00
        bne     invalidfd
        cmp     #MAX_FDS        ; Is it valid?
        bcs     invalidfd       ; Jump if no

; Check if the LFN is valid and the file is open for writing

        adc     #LFN_OFFS       ; Carry is already clear
        tax
        lda     fdtab-LFN_OFFS,x; Get flags for this handle
        and     #LFN_READ       ; File open for writing?
        beq     notopen

; Valid lfn. Make it the input file

        jsr     CHKIN
        bcs     error

; Clear the byte counter

        lda     #$00
        sta     ptr3
        sta     ptr3+1

; Read the status to check if we are already at the end of the file
        
        jsr     READST
        and     #%01000000
        bne     done

; Go looping...

        beq     deccount        ; Branch always

; Read the next byte

loop:   jsr     BASIN
        sta     tmp1            ; Save the input byte

        jsr     READST          ; Read the IEEE status
        sta     tmp2            ; Save it
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

@L2:    lda     tmp2
        and     #%01000000      ; Check for EOI
        bne     done            ; Jump if end of file reached

; Decrement the count

deccount:
        inc     ptr1
        bne     loop
        inc     ptr1+1
        bne     loop

; Read done, close the input channel

done:   jsr     CLRCH

; Return the number of chars read

        lda     ptr3
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




