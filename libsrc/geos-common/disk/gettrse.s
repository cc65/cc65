;
; Maciej 'YTM/Alliance' Witkowiak
;
; 29.1.00

            .export gettrse
            .importzp ptr4

gettrse:
        sta ptr4
        stx ptr4+1
        ldy #1
        lda (ptr4),y
        tax
        dey
        lda (ptr4),y
        rts
