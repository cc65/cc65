;
; Graphics driver for the 80x48x2 (CIO mode 4, ANTIC mode 9) on the Atari.
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
        grmode = 4
; X resolution
        x_res = 80
; Y resolution
        y_res = 48
; Number of colors
        colors = 2
; Pixels per byte
        ppb = 8
; Screen memory size in bytes
        scrsize = x_res * y_res / ppb
; Pixel aspect ratio
        aspect = $00CC                          ; based on 4/3 display
; Free memory needed
        mem_needed = 1
; Number of screen pages
        pages = 1

.rodata
        mask_table:                             ; Mask table to set pixels
                .byte   %10000000, %01000000, %00100000, %00010000, %00001000, %00000100, %00000010, %00000001
        masks:                                  ; Color masks
                .byte   %00000000, %11111111
        bar_table:                              ; Mask table for BAR
                .byte   %11111111, %01111111, %00111111, %00011111, %00001111, %00000111, %00000011, %00000001, %00000000
        default_palette:
                .byte   $00, $0E

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

        ; Done, reset the error code
        lda     #TGI_ERR_OK
        sta     error
        rts
.endproc

.include "atari_tgi_common.inc"
