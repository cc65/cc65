;
; General purpose jump vector in the data segment that is patched at
; runtime and used by several routines.
;
; Ullrich von Bassewitz, 16.12.1998
;

  	.export		jmpvec


.data

jmpvec:	jmp	$FFFF

