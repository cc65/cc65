;
; 2022-03-15, Karri Kaksonen
;
; clock_t _clocks_per_sec (void);
;

        .export         __clocks_per_sec

        .import         sreg: zp
	.import		_paldetected
        .include        "atari7800.inc"

        .macpack        generic

	.code

;-----------------------------------------------------------------------------
; Return the number of clock ticks in one second.
;
        .proc   __clocks_per_sec

        lda     #0
	tax
        sta     sreg            ; return 32 bits
        sta     sreg+1
        lda     _paldetected
	bne	pal
	lda	#60		; NTSC - 60Hz
	rts
pal:
	lda	#50		; PAL - 50Hz
        rts
        .endproc

