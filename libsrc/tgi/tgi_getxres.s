;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned __fastcall__ tgi_getxres (void);
; /* Return the resolution in X direction */


        .include        "tgi-kernel.inc"
        .export         _tgi_getxres
        .import         ldaxidx


_tgi_getxres:
        lda     _tgi_drv
        ldx     _tgi_drv+1
        ldy     #TGI_HDR_XRES+1
        jmp     ldaxidx


