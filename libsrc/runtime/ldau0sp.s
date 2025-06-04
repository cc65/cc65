;
; Ullrich von Bassewitz, 11.04.1999
;
; CC65 runtime: Load an unsigned char indirect from pointer somewhere in stack
;

        .export         ldau00sp, ldau0ysp
        .importzp       c_sp, ptr1

        .macpack        cpu

ldau00sp:
        ldy     #1
ldau0ysp:
        lda     (c_sp),y
        sta     ptr1+1
        dey
        lda     (c_sp),y
        sta     ptr1
        ldx     #0
.if (.cpu .bitand CPU_ISET_65SC02)
        lda     (ptr1)          ; Save one cycle for the C02
.else
        lda     (ptr1,x)
.endif
        rts

