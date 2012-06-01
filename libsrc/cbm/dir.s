;
; Ullrich von Bassewitz, 2012-06-01
;
; unsigned char __fastcall__ _dirskip (unsigned char count, struct DIR* dir);
; /* Skip bytes from the directory and make sure, errno is set if this isn't
;  * possible. Return true if anything is ok and false otherwise. For
;  * simplicity we assume that read will never return less than count if there
;  * is no error and end-of-file is not reached.
;  * Note: count must not be more than 254.
;  */
;

        .include        "dir.inc"
        .include        "errno.inc"
        .include        "zeropage.inc"

        .import         _read
        .import         pushax, pushptr1idx
        .import         subysp, addysp1


.proc   __dirskip

        sta     ptr1
        stx     ptr1+1          ; Save dir

; Get count and allocate space on the stack

        ldy     #0
        lda     (sp),y
        pha
        tay
        jsr     subysp

; Save current value of sp

        lda     sp
        pha
        lda     sp+1
        pha

; Push dir->fd

        ldy     #DIR::fd+1
        jsr     pushptr1idx

; Push pointer to buffer

        pla
        tax
        pla
        jsr     pushax

; Load count and call read

        pla
        pha
        ldx     #0
        jsr     _read

; Check for errors. In case of errors, errno is already set.

        cpx     #$FF
        bne     L2

; read() returned an error

        pla                     ; Count
        tay
        lda     #0
        tax
L1:     jmp     addysp1         ; Drop buffer plus count

; read() was successful, check number of bytes read. We assume that read will
; not return more than count, so X is zero if we come here.

L2:     sta     tmp1
        pla                     ; count
        tay
        cmp     tmp1
        beq     L1              ; Drop variables, return count

; Didn't read enough bytes. This is an error for us, but errno is not set

        lda     #<EIO
        sta     __errno
        stx     __errno+1
        txa                     ; A=X=0
        beq     L1              ; Branch always

.endproc

