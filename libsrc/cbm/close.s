;
; Ullrich von Bassewitz, 16.11.2002
;
; int __fastcall__ close (int fd);
;

        .export         _close

        .import         getdiskerror
        .import         __errno, __oserror

        .include        "errno.inc"
        .include        "cbm.inc"
        .include        "filedes.inc"


;--------------------------------------------------------------------------
; _close

.proc   _close

; Check if we have a valid handle

        cpx     #$00
        bne     invalidfd
        cmp     #MAX_FDS        ; Is it valid?
        bcs     invalidfd       ; Jump if no

; Check if the LFN is valid and the file is open for writing

        adc     #LFN_OFFS       ; Carry is already clear
        tax
        lda     fdtab-LFN_OFFS,x; Get flags for this handle
        beq     notopen

; Valid lfn, close it. The close call is always error free, at least as far
; as the kernal is involved

        lda     #LFN_CLOSED
        sta     fdtab-LFN_OFFS,x
        lda     unittab,x
        pha                     ; Push unit for this file
        txa
        jsr     CLOSE
        pla

; Read the drive error channel

        lda     unittab,x
        tax
        jsr     getdiskerror
        cmp     #$00
        bne     error

; Successful

        tax
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

error:  sta     __oserror
errout: lda     #$FF
        tax                     ; Return -1
        rts

.endproc




