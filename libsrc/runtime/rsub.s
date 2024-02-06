;
; Ullrich von Bassewitz, 05.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: sub ints reversed
;

        .export         tosrsuba0, tosrsubax
        .import         addysp1
        .importzp       sp, tmp1

        .macpack        cpu

;
; AX = AX - TOS
;

tosrsuba0:
        ldx     #0
tosrsubax:
        sec
.if (.cpu .bitand CPU_ISET_65SC02)
        sbc     (sp)
        ldy     #1
.else
        ldy     #0
        sbc     (sp),y          ; lo byte
        iny
.endif
        sta     tmp1            ; save lo byte
        txa
        sbc     (sp),y          ; hi byte
        tax
        lda     tmp1
        jmp     addysp1         ; drop TOS, set condition codes


