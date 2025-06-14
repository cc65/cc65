;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: long add
;

        .export         tosadd0ax, tosaddeax
        .import         addysp1
        .importzp       c_sp, sreg, tmp1

        .macpack        cpu

; EAX = TOS + EAX

tosadd0ax:
        ldy     #$00
        sty     sreg
        sty     sreg+1

tosaddeax:
        clc
.if (.cpu .bitand CPU_ISET_65SC02)
        adc     (c_sp)            ; 65SC02 version - saves 2 cycles
        ldy     #1
.else
        ldy     #0
        adc     (c_sp),y          ; lo byte
        iny
.endif
        sta     tmp1            ; use as temp storage
        txa
        adc     (c_sp),y          ; byte 1
        tax
        iny
        lda     sreg
        adc     (c_sp),y          ; byte 2
        sta     sreg
        iny
        lda     sreg+1
        adc     (c_sp),y          ; byte 3
        sta     sreg+1
        lda     tmp1            ; load byte 0
        jmp     addysp1         ; drop TOS

