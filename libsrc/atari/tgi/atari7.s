;
; Graphics driver for the 160x96x4 (CIO mode 7, ANTIC mode D) on the Atari.
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
        grmode = 7
; X resolution
        x_res = 160
; Y resolution
        y_res = 96
; Number of colors
        colors = 4
; Pixels per byte
        ppb = 4
; Screen memory size in bytes
        scrsize = x_res * y_res / ppb
; Pixel aspect ratio
        aspect = $00CC                          ; based on 4/3 display
; Free memory needed
        mem_needed = 3209
; Number of screen pages
        pages = 1

.rodata
        mask_table:                             ; Mask table to set pixels
                .byte   %11000000, %00110000, %00001100, %00000011
        masks:                                  ; Color masks
                .byte   %00000000, %01010101, %10101010, %11111111
        bar_table:                              ; Mask table for BAR
                .byte   %11111111, %00111111, %00001111, %00000011, %00000000
        default_palette:
                .byte   $00, $0E, $32, $96

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
        dey
        bpl     loop

        ; Get the color entries from the palette
        lda     palette
        sta     COLOR4
        lda     palette + 1
        sta     COLOR0
        lda     palette + 2
        sta     COLOR1
        lda     palette + 3
        sta     COLOR2

        ; Done, reset the error code
        lda     #TGI_ERR_OK
        sta     error
        rts
.endproc

.include "atari_tgi_common.inc"
