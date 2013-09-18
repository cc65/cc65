;
; Christian Groessler, 17-Sep-2013
;
; Override _sys() function for Atari targets:
;   'atari'    gets the regular function
;   'atarixl'  doesn't support the _sys() function
;

.ifndef __ATARIXL__

.include "../common/_sys.s"

.endif
