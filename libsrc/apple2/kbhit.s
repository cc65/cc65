	;;
	;; Kevin Ruland
	;;
	;; int kbhit (void);
	;;

	.export	_kbhit

	.import	return0, return1
	
	.include "apple2.inc"

_kbhit:
	bit	KBD		; Reading keyboard checks for keypress
	bmi	L1		; if KBD > 127 key was pressed
	jmp	return0
L1:
	jmp	return1
