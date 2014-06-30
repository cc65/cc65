;
; Ullrich von Bassewitz, 2012-06-01
;
; Helper functions for open-/read-/closedir


        .include        "dir.inc"
        .include        "errno.inc"
        .include        "zeropage.inc"
                             
        .import         pushax
        .import         _read


;---------------------------------------------------------------------------
;
; unsigned char __fastcall__ _dirread1 (DIR* dir, void* buf);
; /* Read one byte from the directory into the supplied buffer. Makes sure,
; ** errno is set in case of a short read. Return true if the read was
; ** successful and false otherwise.
; */

__dirread1:

        jsr     pushax          ; Push buf
        lda     #1              ; Load count = 1

; Run directly into __dirread

;---------------------------------------------------------------------------
;
; unsigned char __fastcall__ _dirread (DIR* dir, void* buf, unsigned char count);
; /* Read characters from the directory into the supplied buffer. Makes sure,
; ** errno is set in case of a short read. Return true if the read was
; ** successful and false otherwise.
; */

__dirread:

; Save count

        pha

; Replace dir by dir->fd

        ldy     #2
        lda     (sp),y
        sta     ptr1
        iny
        lda     (sp),y
        sta     ptr1+1
        ldy     #DIR::fd+1
        lda     (ptr1),y
        pha
        dey
        lda     (ptr1),y
        ldy     #2
        sta     (sp),y
        pla
        iny
        sta     (sp),y

; Get count, save it again, clear the high byte and call read(). By the
; previous actions, the stack frame is as read() needs it, and read() will
; also drop it.

        pla
        pha
        ldx     #0
        jsr     _read

; Check for errors.

        cpx     #$FF
        bne     L3

; read() returned an error, so errno is already set

        pla                     ; Drop count
        inx                     ; X = 0
L1:     txa                     ; Return zero
L2:     rts

; read() was successful, check number of bytes read. We assume that read will
; not return more than count, so X is zero if we come here.

L3:     sta     tmp1            ; Save returned count
        pla                     ; Our count
        cmp     tmp1
        beq     L2              ; Ok, return count

; Didn't read enough bytes. This is an error for us, but errno is not set

        lda     #<EIO
        sta     __errno
        stx     __errno+1       ; X is zero
        bne     L1              ; Branch always


