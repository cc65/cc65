;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: test long in eax
;

        .export         utsteax, tsteax
        .importzp       sreg, tmp1

tsteax:
utsteax:
        tay                     ; Save value
        stx     tmp1
        ora     tmp1
        ora     sreg
        ora     sreg+1
        beq     L9
        tya
        ldy     #1              ; Force NE
L9:     rts                               


