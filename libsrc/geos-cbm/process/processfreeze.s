
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 27.10.99

;
; void	FreezeProcess	(char number);
; void	UnFreezeProcess	(char number);
;

	    .export _FreezeProcess
	    .export _UnFreezeProcess

	    .include "../inc/jumptab.inc"
	    
_FreezeProcess:
	    tax
	    jmp FreezeProcess
	    
_UnFreezeProcess:
	    tax
	    jmp UnFreezeProcess
