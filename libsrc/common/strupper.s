;
; Ullrich von Bassewitz, 02.06.1998
;
; char* strupper (char* s);
; char* strupr (char* s);
;
; Non-ANSI
;

 	.export		_strupper, _strupr
	.import		popax
	.import		__ctype
	.importzp	ptr1, ptr2

_strupper:
_strupr:
	sta	ptr1		; Save s (working copy)
	stx	ptr1+1
	sta	ptr2
	sta	ptr2+2		; save function result
	ldy	#0

loop:	lda    	(ptr1),y	; get character
	beq	L9		; jump if done
	tax
	lda	__ctype,x	; get character classification
	and    	#$01   	       	; lower case char?
	beq	L1		; jump if no
	txa			; get character back into accu
	clc
       	adc    	#<('A'-'a')     ; make upper case char
	sta	(ptr1),y	; store back
L1:	iny	    		; next char
	bne	loop
	inc	ptr1+1		; handle offset overflow
	bne	loop		; branch always

; Done, return the argument string

L9:	lda	ptr2
	ldx	ptr2+1
	rts



