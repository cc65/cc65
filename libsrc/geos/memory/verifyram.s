
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char VerifyRAM (char REUBank, int length, char *reuaddy, char *cpuaddy);

	    .import REURegs
	    .export _VerifyRAM

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_VerifyRAM:
	    jsr REURegs
	    jsr VerifyRAM
	    txa
	    rts
