;
; 2016-06, Christian Groessler
; 2017-06-26, Greg King
;
; unsigned char doesclrscrafterexit (void);
;
; Returns 0/1 if, after program termination, the screen isn't/is cleared.
;

        .import         return1

; cc65's OSI programs return to the monitor ROM which clears the screen.

        .export         _doesclrscrafterexit := return1
