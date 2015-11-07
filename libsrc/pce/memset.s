;
; This file, instead of "common/memset.s", will be assembled for the pce
; target.  This version is smaller and faster because it uses a HuC6280
; block-copy instruction.
;
; 1998-05-29, Ullrich von Bassewitz
; 2015-11-06, Greg King
;
; void* __fastcall__ _bzero (void* ptr, size_t n);
; void  __fastcall__  bzero (void* ptr, size_t n);
; void* __fastcall__ memset (void* ptr, int c, size_t n);
;
; NOTE: bzero() will return its first argument, as memset() does.  It is no
;       problem to declare the return value as void, because it can be ignored.
;       _bzero() (note the leading underscore) is declared with the proper
;       return type because the compiler will replace memset() by _bzero() if
;       the fill value is zero; and, the optimizer looks at the return type
;       to see if the value in .XA is of any use.
;
; NOTE: This function uses entry points from "pce/memcpy.s"!
;

        .export         __bzero, _bzero, _memset

        .import         memcpy_getparams, memcpy_increment
        .import         pushax, popax
        .importzp       ptr1, ptr2, ptr3

        .macpack        longbranch


; ----------------------------------------------------------------------
__bzero:
_bzero: pha
        cla                             ; fill with zeros
        jsr     pushax                  ; (high byte isn't important)
        pla

_memset:
        jsr     memcpy_getparams

; The fill byte is put at the beginning of the buffer; then, the buffer is
; copied to a second buffer that starts one byte above the start of the first
; buffer.  Normally, we would use memmove() to avoid trouble; but here, we
; exploit that overlap, by using memcpy().  Therefore, the fill value is copied
; from each byte to the next byte, all the way to the end of the buffer.

        lda     ptr1                    ; get fill value
        sta     (ptr2)

        lda     ptr3                    ; count first byte
        bne     @L3
        dec     ptr3+1
@L3:    dec     a
        sta     ptr3
        ora     ptr3+1
        jeq     popax                   ; return ptr. if no more bytes

        lda     ptr2                    ; point to first buffer
        ldx     ptr2+1
        sta     ptr1
        stx     ptr1+1
        inc     ptr2                    ; point to second buffer
        bne     @L2
        inc     ptr2+1

@L2:    jmp     memcpy_increment
