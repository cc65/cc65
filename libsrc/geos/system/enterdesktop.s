
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 30.10.99, 17.04.2003

; void EnterDeskTop (void);

	    .export _EnterDeskTop
	    .import _exit

	    .include "../inc/jumptab.inc"

_EnterDeskTop = _exit
