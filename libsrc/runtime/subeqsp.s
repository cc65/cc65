;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: -= operator for ints on the stack
;

        .export         subeq0sp, subeqysp
        .importzp       spc

subeq0sp:
        ldy     #0
subeqysp:
        sec
        eor     #$FF
        adc     (spc),y
        sta     (spc),y
        pha                     ; Save low byte
        iny
        txa
        eor     #$FF
        adc     (spc),y
        sta     (spc),y
        tax
        pla                     ; Restore low byte
        rts

