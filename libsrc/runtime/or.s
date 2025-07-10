;
; Ullrich von Bassewitz, 05.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: or on ints
;

        .export         tosora0, tosorax
        .import         addysp1
        .importzp       c_sp, tmp1

tosora0:
        ldx     #$00
tosorax:
.if .cap(CPU_HAS_ZPIND)
        ora     (c_sp)
        ldy     #1
.else
        ldy     #0
        ora     (c_sp),y
        iny
.endif
        sta     tmp1
        txa
        ora     (c_sp),y
        tax
        lda     tmp1
        jmp     addysp1         ; drop TOS, set condition codes

