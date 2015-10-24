;
; This file, instead of "common/memcpy.s", will be assembled for the pce
; target.  This version is smaller and faster because it uses the HuC6280's
; block-copy instructions.
;
; 2003-08-20, Ullrich von Bassewitz
; 2015-10-11, Greg King
;
; void* __fastcall__ memcpy (void* dest, const void* src, size_t size);
;
; NOTE:  This function contains entry points for memmove, which will resort
; to memcpy for an incrementing copy.  Don't change this module without looking
; at "pce/memmove.s"!
;

        .export         _memcpy
        .export         memcpy_increment, memcpy_transfer, memcpy_getparams

        .import         popax
        .importzp       sp, ptr1, ptr2, ptr3


; The structure of the transfer instructions

        .struct
opcode          .byte
source          .addr
destination     .addr
length          .word
        .endstruct

; ----------------------------------------------------------------------
_memcpy:
        jsr     memcpy_getparams

memcpy_increment:
        ldy     #$73                    ; TII

memcpy_transfer:
        sty     transfer+opcode

        lda     ptr1
        ldx     ptr1+1
        sta     transfer+source
        stx     transfer+source+1

        lda     ptr2
        ldx     ptr2+1
        sta     transfer+destination
        stx     transfer+destination+1

        lda     ptr3
        ldx     ptr3+1
        sta     transfer+length
        stx     transfer+length+1

        jmp     transfer

; ----------------------------------------------------------------------
; Get the parameters from the stack, as follows:
;
;       size --> ptr3
;       src  --> ptr1
;       dest --> ptr2
;
; The first argument (dest) will remain on the stack; and, is returned in .XA!

memcpy_getparams:
        sta     ptr3
        stx     ptr3+1                  ; save size

        jsr     popax
        sta     ptr1
        stx     ptr1+1                  ; save src

; (Direct stack access is four cycles faster [total cycle count].)

        ldy     #1                      ; save dest
        lda     (sp),y                  ; get high byte
        tax
        lda     (sp)                    ; get low byte
        sta     ptr2
        stx     ptr2+1
        rts                             ; return dest address (for memmove)

; ----------------------------------------------------------------------
; The transfer instructions use inline arguments.
; Therefore, we must build the instruction in the DATA segment.

.data

transfer:
        tii     $FFFF, $FFFF, $0001
        jmp     popax                   ; get pointer; and, return it as result
