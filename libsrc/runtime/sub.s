;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: sub ints
;

        .export         tossuba0, tossubax
        .import         addysp1
        .importzp       sp

        .macpack        cpu

; AX = TOS - AX

tossuba0:
        ldx     #0
tossubax:
        sec
        eor     #$FF
.if (.cpu .bitand CPU_ISET_65SC02)
        adc     (sp)
        ldy     #1
.else
        ldy     #0
        adc     (sp),y          ; Subtract low byte
        iny
.endif
        pha                     ; Save high byte
        txa
        eor     #$FF
        adc     (sp),y          ; Subtract high byte
        tax                     ; High byte into X
        pla                     ; Restore low byte
        jmp     addysp1         ; drop TOS

