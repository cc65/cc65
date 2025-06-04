;
; Ullrich von Bassewitz, 1998-08-21, 2009-02-22
;
; CC65 runtime: Pop TOS into sreg
;

        .export         popsreg
        .import         incsp2
        .importzp       spc, sreg

        .macpack        cpu

popsreg:
        pha                     ; save A
        ldy     #1
        lda     (spc),y          ; get hi byte
        sta     sreg+1          ; store it
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (spc)            ; get lo byte
.else
        dey
        lda     (spc),y          ; get lo byte
.endif
        sta     sreg            ; store it
        pla                     ; get A back
        jmp     incsp2          ; bump stack and return

