;
; Ullrich von Bassewitz, 2003-02-13
;
; void toggle_videomode (void);
; /* Toggle the video mode between 40 and 80 chars (calls SWAPPER) */
;

	.export		_toggle_videomode

	.include	"c128.inc"

	_toggle_videomode = SWAPPER

