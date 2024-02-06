;
; Ullrich von Bassewitz, 2009-10-25
;
; Generates clipping outcodes for Cohen Sutherland and others.
;

        .include "tgi-kernel.inc"
        .include "zeropage.inc"

;----------------------------------------------------------------------------
; Data

.bss

; Clipping coordinates. They must be in this order!
tgi_clip_x1:    .res    2
tgi_clip_y1:    .res    2
tgi_clip_x2:    .res    2
tgi_clip_y2:    .res    2

;----------------------------------------------------------------------------
; Generate a Cohen Sutherland outcode
;
; void outcode ()
; {
;     unsigned char o = 0;
;     if (Y < 0) {
;         o = TGI_CLIP_BOTTOM;
;     } else if (Y >= yres) {
;         o = TGI_CLIP_TOP;
;     }
;     if (X < 0) {
;         o |= TGI_CLIP_LEFT;
;     } else if (X >= xres) {
;         o |= TGI_CLIP_RIGHT;
;     }
;     return o;
; }
;
; The function return the outcode in A and the flags for the outcode are
; correctly set.
;

.code
.proc   tgi_outcode

        lda     #TGI_CLIP_NONE
        sta     tmp1

; Check Y coordinate

        lda     tgi_clip_y1+1,y         ; High byte of Y1
        bmi     L2                      ; Jump if bottom clip

        ldx     tgi_clip_y1,y           ; Low byte of Y1
        cpx     _tgi_yres
        sbc     _tgi_yres+1
        bvs     L1
        eor     #$80
L1:     bpl     L4
        lda     #TGI_CLIP_TOP               ; Top clipping necessary
        bne     L3
L2:     lda     #TGI_CLIP_BOTTOM
L3:     sta     tmp1                    ; Save temp outcode


; Check X coordinate

L4:     lda     tgi_clip_x1+1,y         ; High byte of X1
        bmi     L7                      ; Jump if left clip

        ldx     tgi_clip_x1,y           ; Low byte of X1
        cpx     _tgi_xres
        sbc     _tgi_xres+1
        bvs     L5
        eor     #$80
L5:     bmi     L6

; No right or left clipping necessary

        lda     tmp1
        rts

; Need right clipping

L6:     lda     #TGI_CLIP_RIGHT
        ora     tmp1
        rts

; Need left clipping

L7:     lda     #TGI_CLIP_LEFT
        ora     tmp1
        rts

.endproc


