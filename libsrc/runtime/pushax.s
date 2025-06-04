;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push value in a/x onto the stack
;

        .export         push0, pusha0, pushax
        .importzp       spc

        .macpack        cpu

push0:  lda     #0
pusha0: ldx     #0

; This function is used *a lot*, so don't call any subroutines here.
; Beware: The value in ax must not be changed by this function!
; Beware^2: The optimizer knows about the value of Y after the function
;           returns!

.proc   pushax

        pha                     ; (3)
        lda     spc              ; (6)
        sec                     ; (8)
        sbc     #2              ; (10)
        sta     spc              ; (13)
        bcs     @L1             ; (17)
        dec     spc+1            ; (+5)
@L1:    ldy     #1              ; (19)
        txa                     ; (21)
        sta     (spc),y          ; (27)
        pla                     ; (31)
        dey                     ; (33)
.if (.cpu .bitand ::CPU_ISET_65SC02)
        sta     (spc)            ; (37)
.else
        sta     (spc),y          ; (38)
.endif
        rts                     ; (44/43)

.endproc
