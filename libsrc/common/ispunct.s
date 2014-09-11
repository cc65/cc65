;
; 1998-06-02, Ullrich von Bassewitz
; 2014-09-10, Greg King
;
; int ispunct (int c);
;

        .export         _ispunct
        .include        "ctype.inc"

_ispunct:
        cpx     #>$0000         ; Char range OK?
        bne     @L1             ; Jump if no
        tay
        lda     __ctype,y       ; Get character classification
        and     #CT_NOT_PUNCT   ; Mask relevant bits
        cmp     #1              ; If false, then set "borrow" flag
        lda     #0
        sbc     #0              ; Invert logic
        rts                     ; Return NOT (space | control | digit | alpha)

@L1:    lda     #<0             ; Return false
        tax
        rts

