;
; Ullrich von Bassewitz, 2003-02-13
;
; void toggle_videomode (void);
; /* Toggle the video mode between 40 and 80 chars (calls SWAPPER) */
;

	.export		_toggle_videomode

	.include	"c128.inc"

.proc   _toggle_videomode 

        jsr     SWAPPER                 ; Toggle the mode
 	lda     #14
 	jmp     BSOUT                   ; Switch to lower case chars

.endproc

