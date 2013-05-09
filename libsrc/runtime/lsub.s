;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: long sub
;

;
; EAX = TOS - EAX
;
        .export         tossub0ax, tossubeax
        .import         addysp1
        .importzp       sp, sreg

        .macpack        cpu

tossub0ax:
        ldy     #$00
        sty     sreg
        sty     sreg+1

tossubeax:
        sec
        eor     #$FF
.if (.cpu .bitand CPU_ISET_65SC02)
        adc     (sp)            ; 65SC02 version - saves 2 cycles
        ldy     #1
.else
        ldy     #0
        adc     (sp),y          ; lo byte
        iny
.endif
        pha                     ; Save low byte
        txa
        eor     #$FF
        adc     (sp),y          ; byte 1
        tax
        iny
        lda     (sp),y
        sbc     sreg            ; byte 2
        sta     sreg
        iny
        lda     (sp),y
        sbc     sreg+1          ; byte 3
        sta     sreg+1
        pla                     ; Restore byte 0
        jmp     addysp1         ; Drop TOS

