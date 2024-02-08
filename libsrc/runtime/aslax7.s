;
; Miloslaw Smyk, 2024
;
; CC65 runtime: Scale the primary register by 128, unsigned
;

        .export         shlax7, aslax7

aslax7:
shlax7:                         ; XXXXXXXL AAAAAAAl
        tay
        txa
        lsr                     ; XXXXXXXL -> 0XXXXXXX, L->C
        tya
        ror                     ; AAAAAAAl -> LAAAAAAA, l->C
        tax
        lda     #$00            ; LAAAAAAA 00000000
        ror                     ; LAAAAAAA l0000000
        rts

        ; 10 bytes, 16 cycles + rts
