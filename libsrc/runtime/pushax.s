;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push value in a/x onto the stack
;

        .export         push0, pusha0, pushax
        .importzp       c_sp

push0:  lda     #0
pusha0: ldx     #0

; This function is used *a lot*, so don't call any subroutines here.
; Beware: The value in ax must not be changed by this function!
; Beware^2: The optimizer knows about the value of Y after the function
;           returns!

.proc   pushax

        pha                     ; (3)
        lda     c_sp            ; (6)
        sec                     ; (8)
        sbc     #2              ; (10)
        sta     c_sp            ; (13)
        bcs     @L1             ; (17)
        dec     c_sp+1          ; (+5)
@L1:    ldy     #1              ; (19)
        txa                     ; (21)
        sta     (c_sp),y        ; (27)
        pla                     ; (31)
        dey                     ; (33)
.if .cap(CPU_HAS_ZPIND)
        sta     (c_sp)          ; (37)
.else
        sta     (c_sp),y        ; (38)
.endif
        rts                     ; (44/43)

.endproc
