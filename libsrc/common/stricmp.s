; stricmp.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; int stricmp (const char* s1, const char* s2);         /* DOS way */
; int strcasecmp (const char* s1, const char* s2);      /* UNIX way */
;

        .export         _stricmp, _strcasecmp
        .import         popptr1
        .importzp       ptr1, ptr2, tmp1, tmp2
        .import         ctypemaskdirect
        .include        "ctype.inc"

_stricmp:
_strcasecmp:
        sta     ptr2            ; Save s2
        stx     ptr2+1
        jsr     popptr1         ; get s1
        ; ldy     #0            ; Y=0 guaranteed by popptr1

loop:   lda     (ptr2),y        ; get char from second string
        sta     tmp2            ; and save it
        jsr     ctypemaskdirect ; get character classification
        and     #CT_LOWER       ; lower case char?
        beq     L1              ; jump if no
        lda     #<('A'-'a')     ; make upper case char
        adc     tmp2            ; ctypemaskdirect ensures carry clear!
        sta     tmp2            ; remember upper case equivalent

L1:     lda     (ptr1),y        ; get character from first string
        sta     tmp1
        jsr     ctypemaskdirect ; get character classification
        and     #CT_LOWER       ; lower case char?
        beq     L2              ; jump if no
        lda     #<('A'-'a')     ; make upper case char
        adc     tmp1            ; ctypemaskdirect ensures carry clear!
        sta     tmp1            ; remember upper case equivalent

L2:     ldx     tmp1
        cpx     tmp2            ; compare characters
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
