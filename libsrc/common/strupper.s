;
; 1998-06-02, Ullrich von Bassewitz
; 2013-08-28, Greg King
;
; char* __fastcall__ strupper (char* s);
; char* __fastcall__ strupr (char* s);
;
; Non-ANSI
;

        .export         _strupper, _strupr
        .import         popax
        .import         __ctype
        .importzp       ptr1, ptr2

        .include        "ctype.inc"

_strupper:
_strupr:
        sta     ptr1            ; Save s (working copy)
        stx     ptr1+1
        sta     ptr2
        stx     ptr2+1          ; save function result
        ldy     #0

loop:   lda     (ptr1),y        ; get character
        beq     L9              ; jump if done
        tax
        lda     __ctype,x       ; get character classification
        and     #CT_LOWER       ; lower case char?
        beq     L1              ; jump if no
        txa                     ; get character back into accu
        clc
        adc     #<('A'-'a')     ; make upper case char
        sta     (ptr1),y        ; store back
L1:     iny                     ; next char
        bne     loop
        inc     ptr1+1          ; handle offset overflow
        bne     loop            ; branch always

; Done, return the argument string

L9:     lda     ptr2
        ldx     ptr2+1
        rts



