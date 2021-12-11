;
; Christian Groessler, June-2016
;
; unsigned char doesclrscr(void);
;
; returns 0/1 if after program termination the screen isn't/is cleared
;

        .export _doesclrscrafterexit
        .import return0 

_doesclrscrafterexit = return0
