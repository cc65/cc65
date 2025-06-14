;
; Ullrich von Bassewitz, 05.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: xor on ints
;

        .export         tosxora0, tosxorax
        .import         addysp1
        .importzp       c_sp, tmp1

        .macpack        cpu

tosxora0:
        ldx     #$00
tosxorax:
.if (.cpu .bitand CPU_ISET_65SC02)
        eor     (c_sp)
        ldy     #1
.else
        ldy     #0
        eor     (c_sp),y
        iny
.endif
        sta     tmp1
        txa
        eor     (c_sp),y
        tax
        lda     tmp1
        jmp     addysp1         ; drop TOS, set condition codes

