;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: function prologue
;

        .export         enter
        .importzp       c_sp

enter:  tya                     ; get arg size
        ldy     c_sp
        bne     L1
        dec     c_sp+1
L1:     dec     c_sp
        ldy     #0
        sta     (c_sp),y          ; Store the arg count
        rts

