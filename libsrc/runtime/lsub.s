;
; Ullrich von Bassewitz, 05.08.1998
; Christian Krueger, 11-Mar-2017, ímproved  65SC02 optimization
; CC65 runtime: long sub
;

;
; EAX = TOS - EAX
;
        .export         tossub0ax, tossubeax
        .import         addysp1
        .importzp       spc, sreg

        .macpack        cpu

tossub0ax:
.if (.cpu .bitand ::CPU_ISET_65SC02)
        stz     sreg
        stz     sreg+1
.else
        ldy     #$00
        sty     sreg
        sty     sreg+1
.endif

tossubeax:
        sec
        eor     #$FF
.if (.cpu .bitand ::CPU_ISET_65SC02)
        adc     (spc)            ; 65SC02 version - saves 2 cycles
        ldy     #1
.else
        ldy     #0
        adc     (spc),y          ; lo byte
        iny
.endif
        pha                     ; Save low byte
        txa
        eor     #$FF
        adc     (spc),y          ; byte 1
        tax
        iny
        lda     (spc),y
        sbc     sreg            ; byte 2
        sta     sreg
        iny
        lda     (spc),y
        sbc     sreg+1          ; byte 3
        sta     sreg+1
        pla                     ; Restore byte 0
        jmp     addysp1         ; Drop TOS

