;
; Ullrich von Bassewitz, 08.08.1998
;
; void chlinexy (unsigned char x, unsigned char y, unsigned char length);
; void chline (unsigned char length);
;

	.export 	_chlinexy, _chline, chlinedirect
	.import 	popa, _gotoxy, cputdirect
	.importzp	tmp1

_chlinexy:
       	pha	    		; Save the length
	jsr	popa		; Get y
       	jsr    	_gotoxy		; Call this one, will pop params
	pla			; Restore the length and run into _chline

_chline:
	ldx	#'-' | $80	; Horizontal line, screen code

chlinedirect:
	cmp	#$00		; Is the length zero?
   	beq	done  		; Jump if done
	sta	tmp1
:	txa			; Screen code
   	jsr	cputdirect	; Direct output
   	dec	tmp1
	bne	:-
done:	rts
