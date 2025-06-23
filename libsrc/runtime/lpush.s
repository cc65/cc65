;
; Ullrich von Bassewitz, 06.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: long push
;

;
; push eax on stack
;
        .export         pushl0, push0ax, pusheax
        .import         decsp4
        .importzp       c_sp, sreg

        .macpack        cpu

pushl0:
        lda     #0
        tax
push0ax:
.if (.cpu .bitand ::CPU_ISET_65SC02)
        stz     sreg
        stz     sreg+1
.else
        ldy     #$00
        sty     sreg
        sty     sreg+1
.endif
pusheax:
        pha                     ; decsp will destroy A (but not X)
        jsr     decsp4
        ldy     #3
        lda     sreg+1
        sta     (c_sp),y
        dey
        lda     sreg
        sta     (c_sp),y
        dey
        txa
        sta     (c_sp),y
        pla
.if (.cpu .bitand ::CPU_ISET_65SC02)
        sta     (c_sp)
.else
        dey
        sta     (c_sp),y
.endif
        rts

