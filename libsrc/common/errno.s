;
; Ullrich von Bassewitz, 06.06.1998
;
; int _errno;
;

      	.export		__errno

.bss

__errno:
	.word	0

