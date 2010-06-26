;
; Ullrich von Bassewitz, 16.11.2002
;
; int __fastcall__ close (int fd);
;

        .export         _close

        .import         CLOSE
        .import         readdiskerror, closecmdchannel
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
        beq     invalidfd

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
        ldx     unittab,y       ; Get the disk for this handle
        jsr     readdiskerror   ; Read the disk error code
        pha                     ; Save it on stack
        ldy     tmp2
        ldx     unittab,y
        jsr     closecmdchannel ; Close the disk command channel
        pla                     ; Get the error code from the disk
        jmp     __mappederrno   ; Set _oserror and _errno, return 0/-1

; Error entry: The given file descriptor is not valid or not open

invalidfd:
        lda     #EBADF
        jmp     __directerrno   ; Set _errno, clear _oserror, return -1

.endproc




