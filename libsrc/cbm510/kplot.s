;
; Ullrich von Bassewitz, 13.09.2001
;
; PLOT routine for the 510.
;

      	.export		k_plot
	.import		__VIDRAM_START__
	.importzp	crtc

      	.include      	"cbm510.inc"

	.macpack	generic

; ------------------------------------------------------------------------
;

.proc	k_plot

       	bcs    	get

	stx    	CURS_Y
     	sty    	CURS_X

    	lda    	LineLSBTab,x
    	sta    	SCREEN_PTR
    	sta	CRAM_PTR
    	lda    	LineMSBTab,x
    	sta    	SCREEN_PTR+1
    	sub	#>__VIDRAM_START__
    	add	#>COLOR_RAM
    	sta	CRAM_PTR+1

get:	ldx	CURS_Y
	ldy	CURS_X

	rts

.endproc

; -------------------------------------------------------------------------
; Low bytes of the start address of the screen lines

.rodata

.macro  LineLoTab
	.repeat 25, I
	.byte   <(__VIDRAM_START__ + I * 40)
	.endrep
.endmacro

LineLSBTab:    	LineLoTab

; -------------------------------------------------------------------------
; High bytes of the start address of the screen lines

.macro  LineHiTab
	.repeat 25, I
	.byte   >(__VIDRAM_START__ + I * 40)
	.endrep
.endmacro

LineMSBTab:	LineHiTab


