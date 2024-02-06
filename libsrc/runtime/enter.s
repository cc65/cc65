;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: function prologue
;

        .export         enter
        .importzp       sp

enter:  tya                     ; get arg size
        ldy     sp
        bne     L1
        dec     sp+1
L1:     dec     sp
        ldy     #0
        sta     (sp),y          ; Store the arg count
        rts

