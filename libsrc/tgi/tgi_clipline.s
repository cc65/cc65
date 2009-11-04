;
; Ullrich von Bassewitz, 2009-10-25
;
; Clips the line in ptr1/ptr2/ptr3/ptr4 to the screen coordinates
;


        .export _tgi_clipline
        .export _tgi_clip_x1, _tgi_clip_y1, _tgi_clip_x2, _tgi_clip_y2
        .export _tgi_clip_o1, _tgi_clip_o2      ; Debugging!
        .export _tgi_clip_dx, _tgi_clip_dy
        .export _tgi_xmax, _tgi_ymax

        .import negax, pushax
        .import imul16x16r32, idiv32by16r16
        .import return0, return1

        .include "tgi-kernel.inc"
        .include "zeropage.inc"

        .macpack longbranch

.code

;----------------------------------------------------------------------------
; outcode constants. These aren't really used because most stuff is done by
; shifting the values, but they're here for documentation.

CLIP_NONE       = $00
CLIP_LEFT       = $01
CLIP_RIGHT      = $02
CLIP_BOTTOM     = $04
CLIP_TOP        = $08



;----------------------------------------------------------------------------
; Generate a Cohen Sutherland outcode for tgi_clip_x1/tgi_clip_y1 in _tgi_clip_o1
;
; void outcode1 ()
; {
;     _tgi_clip_o1 = 0;
;     if (Y1 < 0) {
;         _tgi_clip_o1 = CLIP_BOTTOM;
;     } else if (Y1 >= yres) {
;         _tgi_clip_o1 = CLIP_TOP;
;     }
;     if (X1 < 0) {
;         _tgi_clip_o1 |= CLIP_LEFT;
;     } else if (X1 >= xres) {
;         _tgi_clip_o1 |= CLIP_RIGHT;
;     }
; }

.proc   outcode1

        ldy     #CLIP_BOTTOM            ; Assume line needs bottom clip

; Check Y coordinate

        lda     _tgi_clip_y1+1          ; High byte of Y1
        bmi     L2                      ; Jump if bottom clip

        ldy     #CLIP_TOP               ; Assume line needs top clip
        ldx     _tgi_clip_y1            ; Low byte of Y1
        cpx     _tgi_yres
        sbc     _tgi_yres+1
        bvs     L1
        eor     #$80
L1:     bmi     L2             

        ldy     #CLIP_NONE              ; No clipping actually

L2:     sty     _tgi_clip_o1


; Check X coordinate

        ldy     #CLIP_LEFT              ; Assume line needs left clip

        lda     _tgi_clip_x1+1          ; High byte of X1
        bmi     L4                      ; Jump if left clip

        ldy     #CLIP_RIGHT             ; Assume line needs right clip

        ldx     _tgi_clip_x1            ; Low byte of X1
        cpx     _tgi_xres
        sbc     _tgi_xres+1
        bvs     L3
        eor     #$80
L3:     bmi     L4

        ldy     #CLIP_NONE              ; No clipping actually

L4:     tya
        ora     _tgi_clip_o1
        sta     _tgi_clip_o1

        rts

.endproc


;----------------------------------------------------------------------------
; Generate a Cohen Sutherland outcode for tgi_clip_x2/tgi_clip_y2 in _tgi_clip_o2
;
; void outcode2 ()
; {
;     _tgi_clip_o2 = 0;
;     if (Y2 < 0) {
;         _tgi_clip_o2 = CLIP_BOTTOM;
;     } else if (Y2 >= yres) {
;         _tgi_clip_o2 = CLIP_TOP;
;     }
;     if (X2 < 0) {
;         _tgi_clip_o2 |= CLIP_LEFT;
;     } else if (X2 >= xres) {
;         _tgi_clip_o2 |= CLIP_RIGHT;
;     }
; }

.proc   outcode2

        ldy     #CLIP_BOTTOM            ; Assume line needs bottom clip

; Check Y coordinate

        lda     _tgi_clip_y2+1          ; High byte of Y2
        bmi     L2                      ; Jump if bottom clip

        ldy     #CLIP_TOP               ; Assume line needs top clip
        ldx     _tgi_clip_y2            ; Low byte of Y4
        cpx     _tgi_yres
        sbc     _tgi_yres+1
        bvs     L1
        eor     #$80
L1:     bmi     L2

        ldy     #CLIP_NONE              ; No clipping actually

L2:     sty     _tgi_clip_o2


; Check X coordinate

        ldy     #CLIP_LEFT              ; Assume line needs left clip

        lda     _tgi_clip_x2+1          ; High byte of X2
        bmi     L4                      ; Jump if left clip

        ldy     #CLIP_RIGHT             ; Assume line needs right clip

        ldx     _tgi_clip_x2            ; Low byte of X2
        cpx     _tgi_xres
        sbc     _tgi_xres+1
        bvs     L3
        eor     #$80
L3:     bmi     L4

        ldy     #CLIP_NONE              ; No clipping actually

L4:     tya
        ora     _tgi_clip_o2
        sta     _tgi_clip_o2

        rts

.endproc



;----------------------------------------------------------------------------
; Calculate dx and dy
;

.proc   calcdeltas

        lda     _tgi_clip_x2
        sec
        sbc     _tgi_clip_x1
        sta     _tgi_clip_dx
        lda     _tgi_clip_x2+1
        sbc     _tgi_clip_x1+1
        sta     _tgi_clip_dx+1

        lda     _tgi_clip_y2
        sec
        sbc     _tgi_clip_y1
        sta     _tgi_clip_dy
        lda     _tgi_clip_y2+1
        sbc     _tgi_clip_y1+1
        sta     _tgi_clip_dy+1

        rts

.endproc



;----------------------------------------------------------------------------
; Multiplicate value in y/a by dy, then divide by dx.
;

.proc   muldiv_dydx

        sty     ptr1                    ; lhs
        sta     ptr1+1
        lda     _tgi_clip_dy
        ldx     _tgi_clip_dy+1          ; rhs
        jsr     imul16x16r32            ; Multiplicate

; Move the result of the multiplication into ptr1:ptr2

        sta     ptr1
        stx     ptr1+1
        ldy     sreg
        sty     ptr2
        ldy     sreg+1
        sty     ptr2+1

; Load divisor and divide

        lda     _tgi_clip_dx
        ldx     _tgi_clip_dx+1
        jmp     idiv32by16r16

.endproc



;----------------------------------------------------------------------------
; Multiplicate value in y/a by dx, then divide by dy.
;

.proc   muldiv_dxdy

        sty     ptr1                    ; lhs
        sta     ptr1+1
        lda     _tgi_clip_dx
        ldx     _tgi_clip_dx+1          ; rhs
        jsr     imul16x16r32            ; Multiplicate

; Move the result of the multiplication into ptr1:ptr2

        sta     ptr1
        stx     ptr1+1
        ldy     sreg
        sty     ptr2
        ldy     sreg+1
        sty     ptr2+1

; Load divisor and divide

        lda     _tgi_clip_dy
        ldx     _tgi_clip_dy+1
        jmp     idiv32by16r16

.endproc



;----------------------------------------------------------------------------
; Clip a line using Cohen Sutherland
;

.proc   _tgi_clipline

; Generate outcodes

        jsr     outcode1
        jsr     outcode2
        jsr     calcdeltas

; if ((_tgi_clip_o1 | _tgi_clip_o2) == 0) accept;

Loop:   lda     _tgi_clip_o1
        ora     _tgi_clip_o2
        bne     L1
        jmp     return0

; if ((_tgi_clip_o1 & _tgi_clip_o2) != 0) reject;

L1:     lda     _tgi_clip_o1
        and     _tgi_clip_o2
        beq     L2
        jmp     return1

; Check if X1/Y1 needs clipping

L2:     lda     _tgi_clip_o1
        jeq     L10

; Need to clip X1/Y1

        lsr     a                       ; Check for CLIP_LEFT
        bcc     L3

; tgi_clip_y1 += (0 - tgi_clip_x1) * tgi_clip_dy / tgi_clip_dx;
; tgi_clip_x1 = 0;

        lda     #$00
        tax
        beq     L4

L3:     lsr     a                       ; Check for CLIP_RIGHT
        bcc     L5

; tgi_clip_y1 += (tgi_xmax - tgi_clip_x1) * tgi_clip_dy / tgi_clip_dx;
; tgi_clip_x1 = tgi_xmax;

        lda     _tgi_xmax
        ldx     _tgi_xmax+1

L4:     tay
        sec
        sbc     _tgi_clip_x1
        sty     _tgi_clip_x1
        tay
        txa
        sbc     _tgi_clip_x1+1
        stx     _tgi_clip_x1+1

        jsr     muldiv_dydx

        clc
        adc     _tgi_clip_y1
        sta     _tgi_clip_y1
        txa
        adc     _tgi_clip_y1+1
        sta     _tgi_clip_y1+1

;

        lda     _tgi_clip_o1
        lsr     a
        lsr     a
L5:     lsr     a                               ; Check for CLIP_BOTTOM
        bcc     L6

; tgi_clip_x1 = (0 - tgi_clip_y1) * tgi_clip_dx / tgi_clip_dy;
; tgi_clip_y1 = 0;

        lda     #$00
        tax
        beq     L7

L6:     lsr     a                               ; Check for CLIP_TOP
        bcc     L8

; tgi_clip_x1 += (tgi_ymax - tgi_clip_y1) * tgi_clip_dx / tgi_clip_dy;
; tgi_clip_y1 = ymax;

        lda     _tgi_ymax
        ldx     _tgi_ymax+1

L7:     tay
        sec
        sbc     _tgi_clip_y1
        sty     _tgi_clip_y1
        tay
        txa
        sbc     _tgi_clip_y1+1
        stx     _tgi_clip_y1+1

        jsr     muldiv_dxdy

        clc
        adc     _tgi_clip_x1
        sta     _tgi_clip_x1
        txa
        adc     _tgi_clip_x1+1
        sta     _tgi_clip_x1+1

; We need to recalculate outcode1 in this case

L8:     jsr     outcode1

; Check if X2/Y2 needs clipping

L10:    lda     _tgi_clip_o2
        jeq     Loop

; Need to clip X2/Y2

        lsr     a                       ; Check for CLIP_LEFT
        bcc     L11

; tgi_clip_y2 += (0 - tgi_clip_x2) * tgi_clip_dy / tgi_clip_dx;
; tgi_clip_x2 = 0;

        lda     #$00
        tax
        beq     L12

L11:    lsr     a                       ; Check for CLIP_RIGHT
        bcc     L13

; tgi_clip_y2 += (tgi_xmax - tgi_clip_x2) * tgi_clip_dy / tgi_clip_dx;
; tgi_clip_x2 = tgi_xmax;

        lda     _tgi_xmax
        ldx     _tgi_xmax+1

L12:    tay
        sec
        sbc     _tgi_clip_x2
        sty     _tgi_clip_x2
        tay
        txa
        sbc     _tgi_clip_x2+1
        stx     _tgi_clip_x2+1

        jsr     muldiv_dydx

        clc
        adc     _tgi_clip_y2
        sta     _tgi_clip_y2
        txa
        adc     _tgi_clip_y2+1
        sta     _tgi_clip_y2+1

;

        lda     _tgi_clip_o2
        lsr     a
        lsr     a
L13:    lsr     a                       ; Check for CLIP_BOTTOM
        bcc     L14

; tgi_clip_x2 += (0 - tgi_clip_y2) * tgi_clip_dx / tgi_clip_dy;
; tgi_clip_y2 = 0;

        lda     #$00
        tax
        beq     L15

L14:    lsr     a                               ; Check for CLIP_TOP
        bcc     L16

; tgi_clip_x2 += (tgi_ymax - tgi_clip_y2) * tgi_clip_dx / tgi_clip_dy;
; tgi_clip_y2 = tgi_ymax;

        lda     _tgi_ymax
        ldx     _tgi_ymax+1

L15:    tay
        sec
        sbc     _tgi_clip_y2
        sty     _tgi_clip_y2
        tay
        txa
        sbc     _tgi_clip_y2+1
        stx     _tgi_clip_y2+1

        jsr     muldiv_dxdy

        clc
        adc     _tgi_clip_x2
        sta     _tgi_clip_x2
        txa
        adc     _tgi_clip_x2+1
        sta     _tgi_clip_x2+1

; We need to recalculate outcode2 in this case

L16:    jsr     outcode2

; Try again

        jmp     Loop

.endproc




;----------------------------------------------------------------------------
; Data

.bss

_tgi_clip_x1:   .res    2
_tgi_clip_y1:   .res    2
_tgi_clip_x2:   .res    2
_tgi_clip_y2:   .res    2

_tgi_clip_o1:   .res    1
_tgi_clip_o2:   .res    1

_tgi_clip_dx:   .res    2
_tgi_clip_dy:   .res    2

_tgi_xmax:      .res    2
_tgi_ymax:      .res    2
