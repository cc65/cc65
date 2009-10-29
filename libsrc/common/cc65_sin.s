;
; Fixed point sine function.
;
; Returns the cosine for the given argument as angular degree.
; Valid argument range is 0..360
;
;
; Ullrich von Bassewitz, 2009-10-29
;

        .export         _cc65_sin

        .import         _cc65_sintab


; ---------------------------------------------------------------------------
;

.code

.proc   _cc65_sin

; If the high byte is non zero, argument is > 255

        cpx     #0
        bne     L3
        cmp     #180
        bcs     L4

; 0..179°

        cmp     #90
        bcc     L1

; 90..179°. Value is identical to sin(180-val). Carry is set on entry.
;
;       180-val := -val + 180.
; With
;       -val := (val ^ $FF) + 1
; we get
;       180-val = (val ^ $FF) + 1 + 180
; Since carry is set, we can drop the "+ 1".
;

        eor     #$FF
        adc     #180            ; 180-val

; 0..89°. Values for 87..90° are actually 1.0. Since this format doesn't fit
; into the table, we have to check for it manually.

L1:     cmp     #87
        bcc     L2

; The value is 1.0

        ldx     #>(1 << 8)
        lda     #<(1 << 8)
        rts

; 0..86°. Read the value from the table.

L2:     tay
        ldx     #0
        lda     _cc65_sintab,y
        rts

; 180..360°. sin(x) = -sin(x-180). Since the argument is in range 0..180
; after the subtraction, we don't need to handle the high byte.

L3:     sec
L4:     sbc     #180

        cmp     #90
        bcc     L5

; 270..360°. Value is identical to -sin(180-val). Carry is set on entry.
;
;       180-val := -val + 180.
; With
;       -val := (val ^ $FF) + 1
; we get
;       180-val = (val ^ $FF) + 1 + 180
; Since carry is set, we can drop the "+ 1".
;

        eor     #$FF
        adc     #180            ; 180-val

; 180..269°. Values for 267..269° are actually -1.0. Since this format doesn't
; fit into the table, we have to check for it manually.

L5:     ldx     #$FF
        cmp     #87
        bcc     L6

; The value is -1.0

        lda     #<(-1 << 8)
        rts

; 180..266°. Read the value from the table. Carry is clear on entry.

L6:     tay
        txa                     ; A = $FF
        eor     _cc65_sintab,y
        adc     #1
        bcc     L7
        inx
L7:     rts

.endproc


