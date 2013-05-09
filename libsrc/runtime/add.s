;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: add ints
;

; Make this as fast as possible, even if it needs more space since it's
; called a lot!

        .export         tosadda0, tosaddax
        .importzp       sp

        .macpack        cpu

tosadda0:
        ldx     #0
tosaddax:
        clc
.if (.cpu .bitand CPU_ISET_65SC02)
        adc     (sp)            ; 65SC02 version - saves 2 cycles
        ldy     #1
.else
        ldy     #0
        adc     (sp),y          ; lo byte
        iny
.endif
        pha                     ; save it
        txa
        adc     (sp),y          ; hi byte
        tax
        clc
        lda     sp
        adc     #2
        sta     sp
        bcc     L1
        inc     sp+1
L1:     pla                     ; Restore low byte
        rts

