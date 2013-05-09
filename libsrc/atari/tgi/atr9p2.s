;
; Graphics driver for the 80x192x16b (CIO mode 9, ANTIC mode F, GTIA mode $40) on the Atari.
;
; Fatih Aygun (2009)
;

        .include        "atari.inc"
        .include        "zeropage.inc"

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .macpack        generic

; ******************************************************************************

        ; ----------------------------------------------------------------------
        ;
        ; Constants and tables
        ;
        ; ----------------------------------------------------------------------

; Graphics mode
        grmode = 9
; X resolution
        x_res = 80
; Y resolution
        y_res = 192
; Number of colors
        colors = 16
; Pixels per byte
        ppb = 2
; Screen memory size in bytes
        scrsize = x_res * y_res / ppb
; Pixel aspect ratio
        aspect = $0330                          ; based on 4/3 display
; Free memory needed
        mem_needed = 15339
; Number of screen pages
        pages = 2

.rodata
        mask_table:                             ; Mask table to set pixels
                .byte   %11110000, %00001111
        masks:                                  ; Color masks
                .byte   $00, $11, $22, $33, $44, $55, $66, $77, $88, $99, $aa, $bb, $cc, $dd, $ee, $ff
        bar_table:                              ; Mask table for BAR
                .byte   %11111111, %00001111, %00000000
        default_palette:
                .byte   $00, $0F, $01, $02, $03, $04, $05, $06, $07, $08, $09, $0A, $0B, $0C, $0D, $0E

.code

; ******************************************************************************

.proc SETPALETTE

        ; ----------------------------------------------------------------------
        ;
        ; SETPALETTE: Set the palette (in ptr1)
        ;
        ; ----------------------------------------------------------------------

.code
        ; No palettes
        lda     #TGI_ERR_INV_FUNC
        sta     error
        rts
.endproc

.include "atari_tgi_common.inc"
