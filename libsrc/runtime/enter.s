;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: function prologue
;

        .export         enter
        .importzp       spc

enter:  tya                     ; get arg size
        ldy     spc
        bne     L1
        dec     spc+1
L1:     dec     spc
        ldy     #0
        sta     (spc),y          ; Store the arg count
        rts

