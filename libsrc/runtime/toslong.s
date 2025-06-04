;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Convert tos from int to long
;

        .export         tosulong, toslong
        .import         decsp2
        .importzp       spc

        .macpack        cpu

; Convert TOS from int to long

tosulong:
        pha
        jsr     decsp2          ; Make room
        ldy     #2
        lda     (spc),y
.if (.cpu .bitand CPU_ISET_65SC02)
        sta     (spc)            ; 65C02 version
        iny                     ; Y = 3
.else
        ldy     #0
        sta     (spc),y
        ldy     #3
.endif
        lda     (spc),y
toslong1:
        ldy     #1
        sta     (spc),y
        lda     #0              ; Zero extend
toslong2:
        iny
        sta     (spc),y
        iny
        sta     (spc),y
        pla
        rts

toslong:
        pha
        jsr     decsp2          ; Make room
        ldy     #2
        lda     (spc),y
.if (.cpu .bitand CPU_ISET_65SC02)
        sta     (spc)            ; 65C02 version
        iny                     ; Y = 3
.else
        ldy     #0
        sta     (spc),y
        ldy     #3
.endif
        lda     (spc),y
        bpl     toslong1        ; Jump if positive, high word is zero
        ldy     #1
        sta     (spc),y
        lda     #$FF
        bne     toslong2        ; Branch always

