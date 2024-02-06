;
; Ullrich von Bassewitz, 26.10.2000
;
; Screen size variables
;

        .export         screensize

        .include        "c128.inc"

.proc   screensize

        ldx     #40             ; Assume 40 column mode
        bit     MODE
        bpl     C40             ; Jump if 40 column mode
        ldx     #80
C40:    ldy     #25
        rts

.endproc


