;
; Ullrich von Bassewitz, 16.11.2002
;
; int __fastcall__ close (int fd);
;

        .export         _close

        .import         CLOSE
        .import         readdiskerror, closecmdchannel
        .import         __errno, __oserror
        .importzp       tmp2

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
        sta     tmp2            ; Save the handle

; Check if the file is actually open

        tax
        lda     fdtab,x         ; Get flags for this handle
        and     #LFN_OPEN
        beq     notopen

; Valid lfn, close it. The close call is always error free, at least as far
; as the kernal is involved

        lda     #LFN_CLOSED
        sta     fdtab,x
        lda     tmp2            ; Get the handle
        clc
        adc     #LFN_OFFS       ; Make LFN from handle
        jsr     CLOSE

; Read the drive error channel, then close it

        ldy     tmp2            ; Get the handle
        ldx     unittab,y       ; Get teh disk for this handle
        jsr     readdiskerror   ; Read the disk error code
        pha                     ; Save it on stack
        ldy     tmp2
        ldx     unittab,y
        jsr     closecmdchannel ; Close the disk command channel
        pla                     ; Get the error code from the disk
        bne     error           ; Jump if error

; Successful

        tax                     ; Return zero in a/x
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




