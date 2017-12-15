;
; 2016-06, Christian Groessler
; 2017-07-05, Greg King
;
; unsigned char doesclrscrafterexit (void);
;
; Returns 0/1 if, after program termination, the screen isn't/is cleared.
;

        .import         return1

; cc65's CBM510 programs switch to a display screen in the program RAM bank;
; then, they switch back to the system bank when they exit.
; The screen is cleared.

        .export         _doesclrscrafterexit := return1
