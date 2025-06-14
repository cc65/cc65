;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: and on ints
;

        .export         tosanda0, tosandax
        .import         addysp1
        .importzp       c_sp, ptr4

        .macpack        cpu

tosanda0:
        ldx     #$00
tosandax:
.if (.cpu .bitand CPU_ISET_65SC02)
        and     (c_sp)            ; 65SC02 version, saves 2 cycles and 1 byte
        ldy     #1
.else
        ldy     #0
        and     (c_sp),y
        iny
.endif
        pha
        txa
        and     (c_sp),y
        tax
        pla
        jmp     addysp1         ; drop TOS, set condition codes

