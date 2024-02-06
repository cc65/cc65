;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Fetch word indirect and push
;

        .export         pushw, pushwidx, pushptr1idx
        .import         pushax
        .importzp       ptr1


pushw:  ldy     #1
pushwidx:
        sta     ptr1
        stx     ptr1+1
pushptr1idx:
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y
        jmp     pushax

