;
; Christian Groessler, 10.02.2009
; derived from strncmp.s and stricmp.s
;
; int __fastcall__ strnicmp (const char* s1, const char* s2, size_t count);
; int __fastcall__ strncasecmp (const char* s1, const char* s2, size_t count);
;

        .export         _strnicmp, _strncasecmp
        .import         popax, popptr1
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2
        .import         ctypemaskdirect
        .include        "ctype.inc"

_strnicmp:
_strncasecmp:

; Convert the given counter value in a/x from a downward counter into an
; upward counter, so we can increment the counter in the loop below instead
; of decrementing it. This adds some overhead now, but is cheaper than
; executing a more complex test in each iteration of the loop. We do also
; correct the value by one, so we can do the test on top of the loop.

        eor     #$FF
        sta     ptr3
        txa
        eor     #$FF
        sta     ptr3+1

; Get the remaining arguments

        jsr     popax           ; get s2
        sta     ptr2
        stx     ptr2+1
        jsr     popptr1         ; get s1

; Loop setup

        ; ldy     #0            Y=0 guaranteed by popptr1

; Start of compare loop. Check the counter.

Loop:   inc     ptr3
        beq     IncHi           ; increment high byte

; Compare a byte from the strings

Comp:   lda     (ptr2),y
        sta     tmp2            ; remember original char
        jsr     ctypemaskdirect ; get character classification
        and     #CT_LOWER       ; lower case char?
        beq     L1              ; jump if no
        lda     #<('A'-'a')     ; make upper case char
        adc     tmp2            ; ctypemaskdirect ensures carry clear!
        sta     tmp2            ; remember upper case equivalent

L1:     lda     (ptr1),y        ; get character from first string
        sta     tmp1            ; remember original char
        jsr     ctypemaskdirect ; get character classification
        and     #CT_LOWER       ; lower case char?
        beq     L2              ; jump if no
        lda     #<('A'-'a')     ; make upper case char
        adc     tmp1            ; ctypemaskdirect ensures carry clear!
        sta     tmp1            ; remember upper case equivalent

L2:     ldx     tmp1
        cpx     tmp2            ; compare characters
        bne     NotEqual        ; jump if strings different
        txa                     ; end of strings?
        beq     Equal1          ; jump if EOS reached, a/x == 0

; Increment the pointers

        iny
        bne     Loop
        inc     ptr1+1
        inc     ptr2+1
        bne     Loop            ; branch always

; Increment hi byte

IncHi:  inc     ptr3+1
        bne     Comp            ; jump if counter not zero

; Exit code if strings are equal. a/x not set

Equal:  lda     #$00
        tax
Equal1: rts

; Exit code if strings not equal

NotEqual:
        bcs     L3
        ldx     #$FF            ; make result negative
        rts

L3:     ldx     #$01            ; make result positive
        rts
