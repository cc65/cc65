;
; Ullrich von Bassewitz, 24.10.2001
;
; CC65 runtime: Load word from stack slot and push
;

        .export         pushwysp, pushw0sp
        .importzp       c_sp

        .macpack        generic

pushw0sp:
        ldy     #3
pushwysp:
        lda     c_sp              ; 3
        sub     #2              ; 4
        sta     c_sp              ; 3
        bcs     @L1             ; 3(+1)
        dec     c_sp+1            ; (5)
@L1:    lda     (c_sp),y          ; 5 =16
        tax                     ; 2
        dey                     ; 2
        lda     (c_sp),y          ; 5
        ldy     #$00            ; 2
        sta     (c_sp),y          ; 5
        iny                     ; 2
        txa                     ; 2
        sta     (c_sp),y          ; 5
        rts


