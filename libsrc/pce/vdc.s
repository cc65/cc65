        .export         vdc_init

        .include        "pce.inc"

; FIXME: implement selection of different video modes at runtime
HIRES   = 1

vdc_init:
        lda     VDC_CTRL

        VREG    VDC_CR , $0000  ; disable display and interrupts
        VREG    VDC_BXR, $0000  ; no scrolling
        VREG    VDC_BYR, $0000
        VREG    VDC_MWR, $0070  ; 128 x 64 tiles (1024 x 512 pixels)
        VREG    VDC_VSR, $1702  ; CRTC
        VREG    VDC_VDR, $00DF  ; CRTC - VDS
        VREG    VDC_VCR, $000C  ; CRTC - VDE
        VREG    VDC_DCR, $0000

.if      HIRES
        VREG    VDC_HSR, $0C02  ; CRTC
        VREG    VDC_HDR, $043C  ; CRTC - HDS
        lda     #$06
.else
        VREG    VDC_HSR, $0202  ; CRTC
        VREG    VDC_HDR, $041F  ; CRTC - HDS
        lda     #$04
.endif
        sta     VCE_CTRL

        lda     VDC_CTRL
        rts
