;
; Jede (jede@oric.org), 2023-03-13
;

; off_t __fastcall__ lseek(int fd, off_t offset, int whence);

        .export         _lseek

        .include        "telestrat.inc"
        .include        "zeropage.inc"

        .import         popax

.proc _lseek
        ; Save whence
        sta     tmp1
        ; Skip X

        ; Get offset and store

        jsr     popax
        sta     tmp2
        stx     tmp3

        jsr     popax
        sta     RESB
        stx     RESB+1

        ; Get FD
        jsr     popax
        ; Does not need X
        sta     RES           ; Save FD

        lda     tmp2
        ldy     tmp3
        ldx     tmp1          ; Get whence
        BRK_TELEMON XFSEEK
        ; A & X contains position (from 0 to 15 bits)
        ; RES (2 bytes) contains position (from 16 to 31 bits)
        ; Returns long
        pha
        lda     RES+1
        sta     sreg+1
        lda     RES
        sta     sreg
        pla
        rts
.endproc
