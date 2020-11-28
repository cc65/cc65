;
; Ullrich von Bassewitz, 22.06.2002
;
; Original C code by Marc 'BlackJack' Rintsch, 19.03.2001
;
; int __fastcall__ cbm_read (unsigned char lfn, void* buffer, unsigned int size)

        .include        "cbm.inc"

        .export         _cbm_read
        .export         cbm_read_eofs = eofs
        .importzp       ptr1, ptr2, ptr3, tmp1
        .import         popax, popa
        .import         __oserror

MAX_LFNS        =   10          ; Maximum number of logical file numbers open at once.

        .data

eofs:   .res MAX_LFNS

        .code

_cbm_read:
        inx
        stx     ptr1+1
        tax
        inx
        stx     ptr1            ; Save size with each byte incremented separately

        jsr     popax
        sta     ptr2
        stx     ptr2+1          ; Save buffer

        lda     #$00
        sta     ptr3
        sta     ptr3+1          ; bytesread = 0;

        jsr     popa
        pha
        tax
        jsr     CHKIN
        pla
        bcs     @E1             ; Branch on error

; Check if file has been read completely already.

        ldx     #MAX_LFNS - 1
@L5:    cmp eofs,x
        beq     @L4
        dex
        bpl     @L5
        bmi     @L9             ; (jmp)

; Loop

@L1:    jsr     BASIN           ; Read next char from file
        ldy     #0
        sta     (ptr2),y        ; Save read byte

        inc     ptr2
        bne     @L2
        inc     ptr2+1          ; ++buffer;

@L2:    inc     ptr3
        bne     @L3
        inc     ptr3+1          ; ++bytesread;

@L3:    jsr     READST
        cmp     #$40
        beq     @L6             ; Done
        cmp     #0              ; Status ok?
        bne     @E1

@L9:    dec     ptr1
        bne     @L1
        dec     ptr1+1
        bne     @L1

@L4:    jsr     CLRCH

        lda     ptr3
        ldx     ptr3+1          ; return bytesread;

        rts

; EOF reached, add LFN to list of completed files.

@L6:    ldx     #MAX_LFNS - 1
@L7:    lda     eofs,x
        beq     @L8
        dex
        bpl     @L7

; CHKIN failed

@E1:    sta     __oserror
        lda     #$FF
        tax
        rts                     ; return -1

@L8:    lda     $B8             ; LFN
        sta     eofs,x
        bne     @L4             ; (jmp)
