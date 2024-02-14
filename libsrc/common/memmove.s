;
; 2003-08-20, Ullrich von Bassewitz
; 2009-09-13, Christian Krueger -- performance increase (about 20%), 2013-07-25 improved unrolling
; 2015-10-23, Greg King
;
; void* __fastcall__ memmove (void* dest, const void* src, size_t size);
;
; NOTE: This function uses entry points from memcpy!
;

        .export         _memmove, memcpy_upwards
        .import         memcpy_getparams, memcpy_downwards, popax
        .importzp       ptr1, ptr2, ptr3, ptr4, tmp1

        .macpack        generic
        .macpack        longbranch

; ----------------------------------------------------------------------
_memmove:
        jsr     memcpy_getparams

; Check for the copy direction. If dest < src, we must copy upwards (start at
; low addresses and increase pointers), otherwise we must copy downwards
; (start at high addresses and decrease pointers).

        cmp     ptr1
        txa
        sbc     ptr1+1
        jcs     memcpy_downwards  ; Branch if dest < src (upwards copy)

memcpy_upwards:
        ldx     ptr3+1          ; Get high byte of n
        beq     L2              ; Jump if zero

L1:     .repeat 2               ; Unroll this a bit to make it faster...
        lda     (ptr1),Y        ; copy a byte
        sta     (ptr2),Y
        iny
        .endrepeat
        bne     L1
        inc     ptr1+1
        inc     ptr2+1
        dex                     ; Next 256 byte block
        bne     L1              ; Repeat if any

L2:                             ; assert Y = 0
        ldx     ptr3            ; Get the low byte of n
        beq     done            ; something to copy

L3:     lda     (ptr1),Y        ; copy a byte
        sta     (ptr2),Y
        iny
        dex
        bne     L3

; Done, return dest

done:   jmp     popax           ; Pop ptr and return as result
