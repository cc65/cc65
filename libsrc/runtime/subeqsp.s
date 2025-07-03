;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: -= operator for ints on the stack
;

        .export         subeq0sp, subeqysp
        .importzp       c_sp

subeq0sp:
        ldy     #0
subeqysp:
        sec
        eor     #$FF
        adc     (c_sp),y
        sta     (c_sp),y
        pha                     ; Save low byte
        iny
        txa
        eor     #$FF
        adc     (c_sp),y
        sta     (c_sp),y
        tax
        pla                     ; Restore low byte
        rts

