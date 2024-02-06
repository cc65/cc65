;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Convert tos from int to long
;

        .export         tosulong, toslong
        .import         decsp2
        .importzp       sp

        .macpack        cpu

; Convert TOS from int to long

tosulong:
        pha
        jsr     decsp2          ; Make room
        ldy     #2
        lda     (sp),y
.if (.cpu .bitand CPU_ISET_65SC02)
        sta     (sp)            ; 65C02 version
        iny                     ; Y = 3
.else
        ldy     #0
        sta     (sp),y
        ldy     #3
.endif
        lda     (sp),y
toslong1:
        ldy     #1
        sta     (sp),y
        lda     #0              ; Zero extend
toslong2:
        iny
        sta     (sp),y
        iny
        sta     (sp),y
        pla
        rts

toslong:
        pha
        jsr     decsp2          ; Make room
        ldy     #2
        lda     (sp),y
.if (.cpu .bitand CPU_ISET_65SC02)
        sta     (sp)            ; 65C02 version
        iny                     ; Y = 3
.else
        ldy     #0
        sta     (sp),y
        ldy     #3
.endif
        lda     (sp),y
        bpl     toslong1        ; Jump if positive, high word is zero
        ldy     #1
        sta     (sp),y
        lda     #$FF
        bne     toslong2        ; Branch always

