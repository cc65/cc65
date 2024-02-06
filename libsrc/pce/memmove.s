;
; This file, instead of "common/memmove.s", will be assembled for the pce
; target.  This version is smaller and faster because it uses the HuC6280's
; block-copy instructions.
;
; 2003-08-20, Ullrich von Bassewitz
; 2015-10-23, Greg King
;
; void* __fastcall__ memmove (void* dest, const void* src, size_t size);
;
; NOTE: This function uses entry points from "pce/memcpy.s"!
;

        .export         _memmove

        .import         memcpy_getparams, memcpy_increment, memcpy_transfer
        .importzp       ptr1, ptr2, ptr3

        .macpack        generic
        .macpack        longbranch


; ----------------------------------------------------------------------
_memmove:
        jsr     memcpy_getparams

; Check for the copy direction.  If dest < src, we must copy downwards (start
; at low addresses, and increase pointers); otherwise, we must copy upwards
; (start at high addresses, and decrease pointers).

        cmp     ptr1
        txa
        sbc     ptr1+1
        jcc     memcpy_increment        ; Branch if dest < src

; Copy decrementing; adjust the pointers to the end of the memory regions.

        lda     ptr1
        add     ptr3
        sta     ptr1
        lda     ptr1+1
        adc     ptr3+1
        sta     ptr1+1

        lda     ptr1                    ; point to last byte of source
        bne     @L1
        dec     ptr1+1
@L1:    dec     ptr1

        lda     ptr2
        add     ptr3
        sta     ptr2
        lda     ptr2+1
        adc     ptr3+1
        sta     ptr2+1

        lda     ptr2                    ; point to last byte of target
        bne     @L2
        dec     ptr2+1
@L2:    dec     ptr2

        ldy     #$C3                    ; TDD opcode
        jmp     memcpy_transfer
