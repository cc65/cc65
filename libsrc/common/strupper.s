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
        .importzp       ptr1, ptr2
        .import         ctypemaskdirect
        .include        "ctype.inc"

_strupper:
_strupr:
        sta     ptr1            ; save s (working copy)
        stx     ptr1+1
        sta     ptr2
        stx     ptr2+1          ; save function result
        ldy     #0

loop:   lda     (ptr1),y        ; get character
        beq     L9              ; jump if done
        jsr     ctypemaskdirect ; get character classification
        and     #CT_LOWER       ; lower case char?
        beq     L1              ; jump if no
        lda     (ptr1),y        ; fetch character again
        adc     #<('A'-'a')     ; make upper case char (ctypemaskdirect ensures carry clear)
        sta     (ptr1),y        ; store back
L1:     iny                     ; next char
        bne     loop
        inc     ptr1+1          ; handle offset overflow
        bne     loop            ; branch always

; Done, return the argument string

L9:     lda     ptr2
        ldx     ptr2+1
        rts
