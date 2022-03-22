;
; Stefan Haubenthal, 2007-01-21
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using */
;

        .include        "atari7800.inc"
        .include        "get_tv.inc"
        .importzp       tmp1, tmp2
	.import		_paldetected

;--------------------------------------------------------------------------
; _get_tv

.proc   _get_tv

        lda	_paldetected
	beq	ntsc
	lda	#TV::PAL
	rts
ntsc:
	lda	#TV::NTSC
	rts

.endproc
