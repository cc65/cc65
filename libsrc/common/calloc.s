;
; Ullrich von Bassewitz, 15.11.2001
;
; Allocate a block and zero it.
;
; void* __fastcall__ calloc (size_t count, size_t size);
;

        .export _calloc
        .import _malloc, ___bzero
        .import tosumulax, pushax


; -------------------------------------------------------------------------

.proc   _calloc

; We have the first argument in a/x and the second on the stack. Calling
; tosumulax will give the product of both in a/x.

        jsr     tosumulax

; Save size for later

        sta     Size
        stx     Size+1

; malloc() is a fastcall function, so we do already have the argument in
; the right place

        jsr     _malloc

; Check for a NULL pointer

        cpx     #0
        bne     ClearBlock
        cmp     #0
        bne     ClearBlock

; We have a NULL pointer, bail out

        rts

; No NULL pointer, clear the block. _bzero will return a pointer to the
; block which is exactly what we want.

ClearBlock:
        jsr     pushax                  ; ptr
        lda     Size
        ldx     Size+1                  ; Size
        jmp     ___bzero

.endproc

; -------------------------------------------------------------------------
; Data

.bss

Size:   .res    2



