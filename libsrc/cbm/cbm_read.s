;
; Ullrich von Bassewitz, 22.06.2002
;
; Original C code by Marc 'BlackJack' Rintsch, 19.03.2001
;
; int __fastcall__ cbm_read (unsigned char lfn, void* buffer, unsigned int size)
; /* Reads up to "size" bytes from a file to "buffer".
; ** Returns the number of actually read bytes, 0 if there are no bytes left
; ** (EOF) or -1 in case of an error. __oserror contains an errorcode then (see
; ** table below).
; */
; {
;     static unsigned int bytesread;
;     static unsigned char tmp;
;
;     /* if we can't change to the inputchannel #lfn then return an error */
;     if (__oserror = cbm_k_chkin(lfn)) return -1;
;
;     bytesread = 0;
;
;     while (bytesread<size && !cbm_k_readst()) {
;         tmp = cbm_k_basin();
;
;         /* the kernal routine BASIN sets ST to EOF if the end of file
;         ** is reached the first time, then we have store tmp.
;         ** every subsequent call returns EOF and READ ERROR in ST, then
;         ** we have to exit the loop here immediatly.
;         */
;         if (cbm_k_readst() & 0xBF) break;
;
;         ((unsigned char*)buffer)[bytesread++] = tmp;
;     }
;
;     cbm_k_clrch();
;     return bytesread;
; }
;

        .include        "cbm.inc"

        .export         _cbm_read
        .importzp       ptr1, ptr2, ptr3, tmp1
        .import         popax, popa, returnFFFF
        .import         ___oserror


_cbm_read:
        inx
        stx     ptr1+1
        tax
        inx
        stx     ptr1            ; Save size with both bytes incremented separately.

        jsr     popax
        sta     ptr2
        stx     ptr2+1          ; Save buffer

        jsr     popa
        tax
        jsr     CHKIN
        bcs     @E1             ; Branch on error

; bytesread = 0;

        lda     #$00
        sta     ptr3
        sta     ptr3+1
        beq     @L3             ; Branch always

; Loop

@L1:    jsr     READST
        cmp     #0              ; Status ok?
        bne     @L4

        jsr     BASIN           ; Read next char from file
        sta     tmp1            ; Save it for later

        jsr     READST
        and     #$BF
        bne     @L4

        lda     tmp1
        ldy     #0
        sta     (ptr2),y        ; Save read byte

        inc     ptr2
        bne     @L2
        inc     ptr2+1          ; ++buffer;

@L2:    inc     ptr3
        bne     @L3
        inc     ptr3+1          ; ++bytesread;

@L3:    dec     ptr1
        bne     @L1
        dec     ptr1+1
        bne     @L1

@L4:    jsr     CLRCH

        lda     ptr3
        ldx     ptr3+1          ; return bytesread;

        rts

; CHKIN failed

@E1:    sta     ___oserror
        jmp     returnFFFF
