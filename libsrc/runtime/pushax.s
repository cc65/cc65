;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push value in a/x onto the stack
;

        .export         push0, pusha0, pushax
        .importzp       sp

push0:  lda     #0
pusha0: ldx     #0

; This function is used *a lot*, so don't call any subroutines here.
; Beware: The value in ax must not be changed by this function!
; Beware^2: The optimizer knows about the value of Y after the function
;           returns!

.proc   pushax

        pha                     ; (3)
        lda     sp              ; (6)
        sec                     ; (8)
        sbc     #2              ; (10)
        sta     sp              ; (13)
        bcs     @L1             ; (17)
        dec     sp+1            ; (+5)
@L1:    ldy     #1              ; (19)
        txa                     ; (21)
        sta     (sp),y          ; (27)
        pla                     ; (31)
        dey                     ; (33)
        sta     (sp),y          ; (38)
        rts                     ; (44)     

.endproc
