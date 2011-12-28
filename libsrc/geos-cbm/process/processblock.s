
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 27.10.99

;
; void	InitProcesses	(char number, struct process* proctab);
; (rest)
; void	BlockProcess	(char number);
; void	UnBlockProcess	(char number);
;

	    .export _BlockProcess
	    .export _UnBlockProcess

	    .include "../inc/jumptab.inc"
	    
_BlockProcess:
	    tax
	    jmp BlockProcess
	    
_UnBlockProcess:
	    tax
	    jmp UnBlockProcess
