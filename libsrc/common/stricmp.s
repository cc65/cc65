;
; Ullrich von Bassewitz, 03.06.1998
;
; int stricmp (const char* s1, const char* s2);         /* DOS way */
; int strcasecmp (const char* s1, const char* s2);      /* UNIX way */
;

        .export         _stricmp, _strcasecmp
        .import         popax
        .import         __ctype
        .importzp       ptr1, ptr2, tmp1

        .include        "ctype.inc"

_stricmp:
_strcasecmp:
        sta     ptr2            ; Save s2
        stx     ptr2+1
        jsr     popax           ; get s1
        sta     ptr1
        stx     ptr1+1
        ldy     #0

loop:   lda     (ptr2),y        ; get char from second string
        tax
        lda     __ctype,x       ; get character classification
        and     #CT_LOWER       ; lower case char?
        beq     L1              ; jump if no
        txa                     ; get character back
        clc
        adc     #<('A'-'a')     ; make upper case char
        tax                     ;
L1:     stx     tmp1            ; remember upper case equivalent

        lda     (ptr1),y        ; get character from first string
        tax
        lda     __ctype,x       ; get character classification
        and     #CT_LOWER       ; lower case char?
        beq     L2              ; jump if no
        txa                     ; get character back
        clc
        adc     #<('A'-'a')     ; make upper case char
        tax
                                
L2:     cpx     tmp1            ; compare characters
        bne     L3
        txa                     ; end of strings?
        beq     L5              ; a/x both zero
        iny
        bne     loop
        inc     ptr1+1
        inc     ptr2+1
        bne     loop

L3:     bcs     L4
        ldx     #$FF
        rts

L4:     ldx     #$01
L5:     rts
