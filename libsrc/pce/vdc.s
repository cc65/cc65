
        .include        "pce.inc"

; FIXME: implement selection of different video modes at runtime
HIRES   = 1

        .export         vdc_init

vdc_init:
        lda     a:VDC_CTRL

        VREG    $00, $0000      ; MAWR
        VREG    $01, $0000      ; MARR
        VREG    $05, $0000      ; CR
        VREG    $06, $0000      ; RCR
        VREG    $07, $0000      ; BXR
        VREG    $08, $0000      ; BYR
        VREG    $09, $0070      ; MAWR
        VREG    $0C, $1702      ; CRTC - VSR
        VREG    $0D, $00DF      ; CRTC - VDS
        VREG    $0E, $000C      ; CRTC - VDE
        VREG    $0F, $0000      ; DCR

.if      HIRES

        VREG    $0A, $0C02      ; CRTC - HSR
        VREG    $0B, $043C      ; CRTC - HDS
        lda     #$06
        sta     VCE_CTRL

.else

        VREG    $0A, $0202      ; CRTC - HSR
        VREG    $0B, $041F      ; CRTC - HDS
        lda     #$04
        sta     VCE_CTRL

.endif

        lda     a:VDC_CTRL
        rts
