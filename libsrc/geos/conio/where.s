
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001
; 06.03.2002

; unsigned char wherex (void);
; unsigned char wherey (void);

	    .export _wherex, _wherey
	    .importzp cursor_c, cursor_r

	    .include "../inc/jumptab.inc"

_wherex:    lda cursor_c
	    rts

_wherey:    lda cursor_r
	    rts
