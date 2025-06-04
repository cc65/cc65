;
; Ullrich von Bassewitz, 24.10.2001
;
; CC65 runtime: Load word from stack slot and push
;

        .export         pushwysp, pushw0sp
        .importzp       spc

        .macpack        generic

pushw0sp:
        ldy     #3
pushwysp:
        lda     spc              ; 3
        sub     #2              ; 4
        sta     spc              ; 3
        bcs     @L1             ; 3(+1)
        dec     spc+1            ; (5)
@L1:    lda     (spc),y          ; 5 =16
        tax                     ; 2
        dey                     ; 2
        lda     (spc),y          ; 5
        ldy     #$00            ; 2
        sta     (spc),y          ; 5
        iny                     ; 2
        txa                     ; 2
        sta     (spc),y          ; 5
        rts


