	;;
	;; Kevin Ruland
	;;
	;; int kbhit (void);
	;;

	.export	_kbhit

	.import	return0, return1
	
	.include "apple2.inc"

_kbhit:
	bit	KEY_STROBE	; Reading strobe checks for keypress
	bmi	L1		; if KEY_STROBE > 127 key was pressed
	jmp	return0
L1:
	jmp	return1
