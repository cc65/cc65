
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char FollowChain  (struct tr_se *myTrSe, char *buffer);

	    .export _FollowChain
	    .import popax, __oserror
	    .import gettrse

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_FollowChain:
	sta r3L
	stx r3H
	jsr popax
	jsr gettrse
	sta r1L
	stx r1H
	jsr FollowChain
	stx __oserror
	txa
	rts
