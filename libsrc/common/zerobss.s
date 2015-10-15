;
; Ullrich von Bassewitz, 1998-09-17, 2005-02-26.
;
; Zero the bss segment.
;

        .export         zerobss
        .import         __BSS_RUN__, __BSS_SIZE__
        .importzp       ptr1


.code

zerobss:
        lda     #<__BSS_RUN__
        sta     ptr1
        lda     #>__BSS_RUN__
        sta     ptr1+1
        lda     #0
        tay

; Clear full pages

L1:     ldx     #>__BSS_SIZE__
        beq     L3
L2:     sta     (ptr1),y
        iny
        bne     L2
        inc     ptr1+1
        dex
        bne     L2

; Clear remaining page (y is zero on entry)

L3:     cpy     #<__BSS_SIZE__
        beq     L4
        sta     (ptr1),y
        iny
        bne     L3

; Done

L4:     rts
