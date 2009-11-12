;
; Ullrich von Bassewitz, 2009-10-25
;
; Clips line coordinates to the screen coordinates and calls tgi_line
;


        .import umul16x16r32, udiv32by16r16
        .import negax

        .include "tgi-kernel.inc"
        .include "zeropage.inc"

        .macpack longbranch

;----------------------------------------------------------------------------
; Data

.bss

; Outcodes for both ends
tgi_clip_o1:    .res    1
tgi_clip_o2:    .res    1

; Line deltas
tgi_clip_d:     .res    1
tgi_clip_dx:    .res    2
tgi_clip_dy:    .res    2

tgi_clip_sign:  .res    1


;----------------------------------------------------------------------------
; Calculate outcodes for both ends of the line
;

.code
.proc   outcode1

        ldy     #0
        jsr     tgi_outcode
        sta     tgi_clip_o1
        rts

.endproc

.code
.proc   outcode2

        ldy     #<(tgi_clip_y2 - tgi_clip_y1)
        jsr     tgi_outcode
        sta     tgi_clip_o2
        rts

.endproc


;----------------------------------------------------------------------------
; Negate tgi_clip_dxy
;

.code
.proc   negate

        lda     tgi_clip_dx,y
        eor     #$FF
        clc
        adc     #1
        sta     tgi_clip_dx,y
        lda     tgi_clip_dx+1,y
        eor     #$FF
        adc     #$00
        sta     tgi_clip_dx+1,y
        rts

.endproc


;----------------------------------------------------------------------------
; Calculate the absolute values of dx and dy and store the combined sign in
; tgi_clip_sign
;

.code
.proc   calcdeltas

        lda     tgi_clip_x2
        sec
        sbc     tgi_clip_x1
        sta     tgi_clip_dx
        lda     tgi_clip_x2+1
        sbc     tgi_clip_x1+1
        sta     tgi_clip_dx+1
        sta     tgi_clip_sign
        bpl     @L1
        ldy     #0
        jsr     negate

@L1:    lda     tgi_clip_y2
        sec
        sbc     tgi_clip_y1
        sta     tgi_clip_dy
        lda     tgi_clip_y2+1
        sbc     tgi_clip_y1+1
        sta     tgi_clip_dy+1

        eor     tgi_clip_sign
        sta     tgi_clip_sign

        bit     tgi_clip_dy+1
        bpl     @L9

        ldy     #(tgi_clip_dy - tgi_clip_dx)
        jmp     negate

@L9:    rts

.endproc


;----------------------------------------------------------------------------
; Helper routine. Generate the absolute value of y/a and calculate the sign
; of the final result
;

.code
.proc   prepare_coord

        tax                             ; Remember high byte
        eor     tgi_clip_sign
        sta     tmp1                    ; Sign of result
        tya
        cpx     #0                      ; Check sign
        bpl     @L1
        jsr     negax
@L1:    sta     ptr1
        stx     ptr1+1
        rts

.endproc


;----------------------------------------------------------------------------
; Helper routine. Move the value in eax to ptr1:ptr2
;

.code
.proc   move_intermediate_result

        sta     ptr1
        stx     ptr1+1
        ldy     sreg
        sty     ptr2
        ldy     sreg+1
        sty     ptr2+1
        rts

.endproc


;----------------------------------------------------------------------------
; Multiplicate value in y/a by dy, then divide by dx.
;

.code
.proc   muldiv_dydx

; Generate the absolute value of y/a and calculate the sign of the final
; result

        jsr     prepare_coord

; All values are positive now (dx/dy have been made positive in calcdeltas)
; and the sign of the final result is on tmp1, so we can use unsigned
; operations and apply the final result later, after rounding.

        lda     tgi_clip_dy
        ldx     tgi_clip_dy+1           ; rhs
        jsr     umul16x16r32            ; Multiplicate

; Move the result of the multiplication into ptr1:ptr2

        jsr     move_intermediate_result

; Load divisor and divide

        lda     tgi_clip_dx
        ldx     tgi_clip_dx+1
        jsr     udiv32by16r16

; Check the sign of the final result and negate it if nessary

done:   bit     tmp1
        jmi     negax
        rts

.endproc



;----------------------------------------------------------------------------
; Multiplicate value in y/a by dx, then divide by dy.
;

.code
.proc   muldiv_dxdy

; Generate the absolute value of y/a and calculate the sign of the final
; result

        jsr     prepare_coord

; All values are positive now (dx/dy have been made positive in calcdeltas)
; and the sign of the final result is on tmp1, so we can use unsigned
; operations and apply the final result later, after rounding.

        lda     tgi_clip_dx
        ldx     tgi_clip_dx+1           ; rhs
        jsr     umul16x16r32            ; Multiplicate

; Move the result of the multiplication into ptr1:ptr2

        jsr     move_intermediate_result

; Load divisor and divide

        lda     tgi_clip_dy
        ldx     tgi_clip_dy+1
        jsr     udiv32by16r16

; Check the sign of the final result and negate it if nessary

        jmp     muldiv_dydx::done

.endproc



;----------------------------------------------------------------------------
; Clip a line using Cohen Sutherland
;

.code
.proc   tgi_clippedline

; Set a flag that we have no deltas calculated

        lda     #0
        sta     tgi_clip_d

; Generate outcodes

        jsr     outcode1
        jsr     outcode2

; if ((tgi_clip_o1 | tgi_clip_o2) == 0) {
;     tgi_line (x1, y1, x2, y2);
; }

Loop:   lda     tgi_clip_o1
        ora     tgi_clip_o2
        bne     L1

; Copy the coordinates into ptr1-4 and draw the line

        ldx     #7
L0:     lda     tgi_clip_x1,x
        sta     ptr1,x
        dex
        bpl     L0
        jmp     tgi_line

; if ((tgi_clip_o1 & tgi_clip_o2) != 0) reject;

L1:     lda     tgi_clip_o1
        and     tgi_clip_o2
        beq     L2
        rts                             ; Nothing to draw

; We must clip. If we haven't already done so, calculate dx/dy.

L2:     lda     tgi_clip_d              ; Deltas alreay calculated?
        bne     HaveDeltas              ; Jump if yes
        inc     tgi_clip_d
        jsr     calcdeltas

; Check if X1/Y1 needs clipping

HaveDeltas:
        lda     tgi_clip_o1
        jeq     L10

; Need to clip X1/Y1

        lsr     a                       ; Check for TGI_CLIP_LEFT
        bcc     L3

; tgi_clip_y1 += (0 - tgi_clip_x1) * tgi_clip_dy / tgi_clip_dx;
; tgi_clip_x1 = 0;

        lda     #$00
        tax
        beq     L4

L3:     lsr     a                       ; Check for TGI_CLIP_RIGHT
        bcc     L5

; tgi_clip_y1 += (tgi_xmax - tgi_clip_x1) * tgi_clip_dy / tgi_clip_dx;
; tgi_clip_x1 = tgi_xmax;

        lda     _tgi_xmax
        ldx     _tgi_xmax+1

L4:     tay
        sec
        sbc     tgi_clip_x1
        sty     tgi_clip_x1
        tay
        txa
        sbc     tgi_clip_x1+1
        stx     tgi_clip_x1+1

        jsr     muldiv_dydx

        clc
        adc     tgi_clip_y1
        sta     tgi_clip_y1
        txa
        adc     tgi_clip_y1+1
        sta     tgi_clip_y1+1

;

        lda     tgi_clip_o1
        lsr     a
        lsr     a
L5:     lsr     a                               ; Check for TGI_CLIP_BOTTOM
        bcc     L6

; tgi_clip_x1 = (0 - tgi_clip_y1) * tgi_clip_dx / tgi_clip_dy;
; tgi_clip_y1 = 0;

        lda     #$00
        tax
        beq     L7

L6:     lsr     a                               ; Check for TGI_CLIP_TOP
        bcc     L8

; tgi_clip_x1 += (tgi_ymax - tgi_clip_y1) * tgi_clip_dx / tgi_clip_dy;
; tgi_clip_y1 = ymax;

        lda     _tgi_ymax
        ldx     _tgi_ymax+1

L7:     tay
        sec
        sbc     tgi_clip_y1
        sty     tgi_clip_y1
        tay
        txa
        sbc     tgi_clip_y1+1
        stx     tgi_clip_y1+1

        jsr     muldiv_dxdy

        clc
        adc     tgi_clip_x1
        sta     tgi_clip_x1
        txa
        adc     tgi_clip_x1+1
        sta     tgi_clip_x1+1

; We need to recalculate outcode1 in this case

L8:     jsr     outcode1

; Check if X2/Y2 needs clipping

L10:    lda     tgi_clip_o2
        jeq     Loop

; Need to clip X2/Y2

        lsr     a                       ; Check for TGI_CLIP_LEFT
        bcc     L11

; tgi_clip_y2 += (0 - tgi_clip_x2) * tgi_clip_dy / tgi_clip_dx;
; tgi_clip_x2 = 0;

        lda     #$00
        tax
        beq     L12

L11:    lsr     a                       ; Check for TGI_CLIP_RIGHT
        bcc     L13

; tgi_clip_y2 += (tgi_xmax - tgi_clip_x2) * tgi_clip_dy / tgi_clip_dx;
; tgi_clip_x2 = tgi_xmax;

        lda     _tgi_xmax
        ldx     _tgi_xmax+1

L12:    tay
        sec
        sbc     tgi_clip_x2
        sty     tgi_clip_x2
        tay
        txa
        sbc     tgi_clip_x2+1
        stx     tgi_clip_x2+1

        jsr     muldiv_dydx

        clc
        adc     tgi_clip_y2
        sta     tgi_clip_y2
        txa
        adc     tgi_clip_y2+1
        sta     tgi_clip_y2+1

;

        lda     tgi_clip_o2
        lsr     a
        lsr     a
L13:    lsr     a                       ; Check for TGI_CLIP_BOTTOM
        bcc     L14

; tgi_clip_x2 += (0 - tgi_clip_y2) * tgi_clip_dx / tgi_clip_dy;
; tgi_clip_y2 = 0;

        lda     #$00
        tax
        beq     L15

L14:    lsr     a                       ; Check for TGI_CLIP_TOP
        bcc     L16

; tgi_clip_x2 += (tgi_ymax - tgi_clip_y2) * tgi_clip_dx / tgi_clip_dy;
; tgi_clip_y2 = tgi_ymax;

        lda     _tgi_ymax
        ldx     _tgi_ymax+1

L15:    tay
        sec
        sbc     tgi_clip_y2
        sty     tgi_clip_y2
        tay
        txa
        sbc     tgi_clip_y2+1
        stx     tgi_clip_y2+1

        jsr     muldiv_dxdy

        clc
        adc     tgi_clip_x2
        sta     tgi_clip_x2
        txa
        adc     tgi_clip_x2+1
        sta     tgi_clip_x2+1

; We need to recalculate outcode2 in this case

L16:    jsr     outcode2

; Try again

        jmp     Loop

.endproc




