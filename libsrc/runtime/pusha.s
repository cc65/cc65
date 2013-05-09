;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push value in a onto the stack
;

        .export         pusha0sp, pushaysp, pusha
        .importzp       sp

        .macpack        cpu

; Beware: The optimizer knows about this function!

pusha0sp:
        ldy     #$00
pushaysp:
        lda     (sp),y
pusha:  ldy     sp              ; (3)
        beq     @L1             ; (6)
        dec     sp              ; (11)
        ldy     #0              ; (13)
        sta     (sp),y          ; (19)
        rts                     ; (25)

@L1:    dec     sp+1            ; (11)
        dec     sp              ; (16)
        sta     (sp),y          ; (22)
        rts                     ; (28)

