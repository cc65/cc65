;
; Ullrich von Bassewitz, 05.08.1998
; Christian Krueger, 11-Mar-2017, spend two bytes for one cycle, improved 65SC02 optimization
;
; CC65 runtime: add ints
;

; Make this as fast as possible, even if it needs more space since it's
; called a lot!

        .export         tosadda0, tosaddax
        .importzp       spc, tmp1

        .macpack        cpu

tosadda0:
        ldx     #0
tosaddax:
        clc                     ; (2)

.if (.cpu .bitand ::CPU_ISET_65SC02)

        adc     (spc)            ; (7)
        tay                     ; (9)
        inc     spc              ; (14)
        bne     hiadd           ; (17)
        inc     spc+1            ; (-1+5)
hiadd:  txa                     ; (19)
        adc     (spc)            ; (24)
        tax                     ; (26)
        inc     spc              ; (31)
        bne     done            ; (34)
        inc     spc+1            ; (-1+5)
done:   tya                     ; (36)

.else

        ldy     #0              ; (4)
        adc     (spc),y          ; (9) lo byte
        iny                     ; (11)
        sta     tmp1            ; (14) save it
        txa                     ; (16)
        adc     (spc),y          ; (21) hi byte
        tax                     ; (23)
        clc                     ; (25)
        lda     spc              ; (28)
        adc     #2              ; (30)
        sta     spc              ; (33)
        bcc     L1              ; (36)
        inc     spc+1            ; (-1+5)
L1:     lda     tmp1            ; (39) restore low byte

.endif
        rts                     ; (6502: 45 cycles, 26 bytes <-> 65SC02: 42 cycles, 22 bytes )
