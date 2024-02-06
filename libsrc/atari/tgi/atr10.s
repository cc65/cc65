;
; Graphics driver for the 80x192x9 (CIO mode 10, ANTIC mode F, GTIA mode $80) on the Atari.
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
        grmode = 10
; X resolution
        x_res = 80
; Y resolution
        y_res = 192
; Number of colors
        colors = 9
; Pixels per byte
        ppb = 2
; Screen memory size in bytes
        scrsize = x_res * y_res / ppb
; Pixel aspect ratio
        aspect = $0330                          ; based on 4/3 display
; Free memory needed
        mem_needed = 7147
; Number of screen pages
        pages = 1

.rodata
        mask_table:                             ; Mask table to set pixels
                .byte   %11110000, %00001111
        masks:                                  ; Color masks
                .byte   $00, $11, $22, $33, $44, $55, $66, $77, $88
        bar_table:                              ; Mask table for BAR
                .byte   %11111111, %00001111, %00000000
        default_palette:
                .byte   $00, $0E, $32, $96, $68, $C4, $74, $EE, $4A

.code

; ******************************************************************************

.proc SETPALETTE

        ; ----------------------------------------------------------------------
        ;
        ; SETPALETTE: Set the palette (in ptr1)
        ;
        ; ----------------------------------------------------------------------

.code
        ; Copy the palette
        ldy     #colors - 1
loop:   lda     (ptr1),y
        sta     palette,y
        sta     PCOLR0,y
        dey
        bpl     loop

        ; Done, reset the error code
        lda     #TGI_ERR_OK
        sta     error
        rts
.endproc

.include "atari_tgi_common.inc"
