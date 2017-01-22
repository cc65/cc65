;
; Ullrich von Bassewitz, 2003-04-13
;
; void hires(void);
;
; This function is a hack!
;

        .export         _hires


        .include        "telemon24.inc"

		
; can be optimized with a macro		
.proc   _hires
	brk
    .byt $1a
	rts
.endproc


