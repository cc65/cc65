;
; save and restore system vectors
; originally by Mark Keates
;
; void _save_vecs(void);
; void _rest_vecs(void);
;

        .export __save_vecs,__rest_vecs
.include        "atari.inc"

        .bss

old_dli:     .res 2
old_dlist:   .res 2
old_vbi:     .res 2
old_vbd:     .res 2
old_gra:     .res 1
old_dma:     .res 1
old_prior:   .res 1
old_cols:    .res 8
old_set:     .res 1
old_rmargin: .res 1     ; lmargin saved in startup code

        .code

.proc   __save_vecs

        lda     VDSLST
        sta     old_dli
        lda     VDSLST+1
        sta     old_dli+1
        lda     SDLSTL
        sta     old_dlist
        lda     SDLSTL+1
        sta     old_dlist+1
        lda     VVBLKI
        sta     old_vbi
        lda     VVBLKI+1
        sta     old_vbi+1
        lda     VVBLKD
        sta     old_vbd
        lda     VVBLKD+1
        sta     old_vbd+1
        lda     GRACTL
        sta     old_gra
        lda     SDMCTL
        sta     old_dma
        lda     GPRIOR
        sta     old_prior
        lda     CHBAS
        sta     old_set
        lda     RMARGN
        sta     old_rmargin

        ldy     #7
SETUP1:
        lda     PCOLR0,y
        sta     old_cols,y
        dey
        bpl     SETUP1
        rts

.endproc

.proc   __rest_vecs

        lda     #6
        ldx     old_vbi+1
        ldy     old_vbi
        jsr     SETVBV
        lda     #7
        ldx     old_vbd+1
        ldy     old_vbd
        jsr     SETVBV
        lda     old_dli
        sta     VDSLST
        lda     old_dli+1
        sta     VDSLST+1
        lda     old_dlist
        sta     SDLSTL
        lda     old_dlist+1
        sta     SDLSTL+1
        lda     old_gra
        sta     GRACTL
        lda     old_prior
        sta     GPRIOR
        lda     old_dma
        sta     SDMCTL
        lda     old_set
        sta     CHBAS
        lda     old_rmargin
        sta     RMARGN
        lda     #$FF
        sta     CH
        ldy     #7
SETUP2:
        lda     old_cols,Y
        sta     PCOLR0,Y
        dey
        bpl     SETUP2
        rts

.endproc

