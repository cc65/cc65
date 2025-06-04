;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push value in a onto the stack
;

        .export         pusha0sp, pushaysp, pusha
        .importzp       spc

        .macpack        cpu

; Beware: The optimizer knows about this function!

pusha0sp:
        ldy     #$00
pushaysp:
        lda     (spc),y
pusha:  ldy     spc              ; (3)
        beq     @L1             ; (6)
        dec     spc              ; (11)
        ldy     #0              ; (13)
        sta     (spc),y          ; (19)
        rts                     ; (25)

@L1:    dec     spc+1            ; (11)
        dec     spc              ; (16)
        sta     (spc),y          ; (22)
        rts                     ; (28)

