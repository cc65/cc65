;
; Ullrich von Bassewitz, 15.11.2002
;
; Original C code by Marc 'BlackJack' Rintsch, 25.03.2001
;
; int __fastcall__ cbm_write(unsigned char lfn, void* buffer, unsigned int size)
; {
;
;     static unsigned int byteswritten;
;
;     /* if we can't change to the outputchannel #lfn then return an error */
;     if (_oserror = cbm_k_ckout(lfn)) return -1;
;
;     byteswritten = 0;
;
;     while (byteswritten<size && !cbm_k_readst()) {
;         cbm_k_bsout(((unsigned char*)buffer)[byteswritten++]);
;     }
;
;     if (cbm_k_readst()) {
;         _oserror = 5;       /* device not present */
;         byteswritten = -1;
;     }
;
;     cbm_k_clrch();
;
;     return byteswritten;
; }
;

        .include        "cbm.inc"

        .export         _cbm_write
        .import         CKOUT, READST, BSOUT, CLRCH
        .importzp       ptr1, ptr2, ptr3
        .import         popax, popa
        .import         __oserror
                                                   

_cbm_write:
        sta     ptr3
        stx     ptr3+1          ; Save size
        eor     #$FF
        sta     ptr1
        txa
        eor     #$FF
        sta     ptr1+1          ; Save -size-1

        jsr     popax
        sta     ptr2
        stx     ptr2+1          ; Save buffer

        jsr     popa
        tax
        jsr     CKOUT
        bcs     @E2             ; Branch on error
        bcc     @L3             ; Branch always

; Loop

@L1:    jsr     READST
        cmp     #0              ; Status ok?
        bne     @E1

        ldy     #0
        lda     (ptr2),y        ;
        inc     ptr2
        bne     @L2
        inc     ptr2+1          ; A = *buffer++;

@L2:    jsr     BSOUT           ; cbm_k_bsout (A);

@L3:    inc     ptr1            ; --size;
        bne     @L1
        inc     ptr1+1
        bne     @L1

        jsr     CLRCH

        lda     ptr3
        ldx     ptr3+1          ; return size;

        rts

; Error entry, called when READST fails

@E1:    lda     #5

; Error entry, error code is in A

@E2:    sta     __oserror
        lda     #$FF
        tax
        rts                     ; return -1

