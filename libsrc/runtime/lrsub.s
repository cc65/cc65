;
; Ullrich von Bassewitz, 05.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: long sub reversed
;

;
; EAX = EAX - TOS
;
        .export         tosrsub0ax, tosrsubeax
        .import         addysp1
        .importzp       c_sp, sreg, tmp1

tosrsub0ax:
.if .cap(CPU_HAS_STZ)
        stz     sreg
        stz     sreg+1
.else
        ldy     #$00
        sty     sreg
        sty     sreg+1
.endif

tosrsubeax:
        sec
.if .cap(CPU_HAS_ZPIND)
        sbc     (c_sp)
        ldy     #1
.else
        ldy     #0
        sbc     (c_sp),y        ; byte 0
        iny
.endif
        sta     tmp1            ; use as temp storage
        txa
        sbc     (c_sp),y        ; byte 1
        tax
        iny
        lda     sreg
        sbc     (c_sp),y        ; byte 2
        sta     sreg
        iny
        lda     sreg+1
        sbc     (c_sp),y        ; byte 3
        sta     sreg+1
        lda     tmp1
        jmp     addysp1         ; drop TOS

