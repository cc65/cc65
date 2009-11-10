;
; Graphics driver for the 80x192x16h (CIO mode 11, ANTIC mode F, GTIA mode $C0) on the Atari.
;
; Fatih Aygun (2009)
;

	.include	"atari.inc"
	.include 	"zeropage.inc"

	.include 	"tgi-kernel.inc"
	.include        "tgi-mode.inc"
	.include        "tgi-error.inc"

	.macpack        generic

; ******************************************************************************

	; ----------------------------------------------------------------------
	;
	; Constants and tables
	;
	; ----------------------------------------------------------------------

; Graphics mode
	.define grmode 11
; X resolution
	.define x_res 80
; Y resolution
	.define y_res 192
; Number of colors
	.define	colors 16
; Pixels per byte
	.define	ppb 2
; Screen memory size in bytes
	.define	scrsize x_res * y_res / ppb
; Pixel aspect ratio
	.define	aspect $0400				; 4:1
; Free memory needed
	.define	mem_needed 7147
; Number of screen pages
	.define	pages 1

.rodata
	mask_table:				; Mask table to set pixels
		.byte	%11110000, %00001111
	masks:					; Color masks
		.byte	$00, $11, $22, $33, $44, $55, $66, $77, $88, $99, $aa, $bb, $cc, $dd, $ee, $ff
	bar_table:				; Mask table for BAR
		.byte	%11111111, %00001111, %00000000
	default_palette:
		.byte	$00, $10, $20, $30, $40, $50, $60, $70, $80, $90, $A0, $B0, $C0, $D0, $E0, $F0

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
