;
; Ullrich von Bassewitz, 08.08.1998
;
; void cclearxy (unsigned char x, unsigned char y, unsigned char length);
; void cclear (unsigned char length);
;

    	.export		_cclearxy, _cclear
	.import 	popa, _gotoxy, chlinedirect

_cclearxy:
       	pha	    		; Save the length
	jsr	popa		; Get y
       	jsr    	_gotoxy		; Call this one, will pop params
       	pla			; Restore the length and run into _cclear

_cclear:
	ldx	#' ' | $80	; Blank, screen code
	jmp	chlinedirect
