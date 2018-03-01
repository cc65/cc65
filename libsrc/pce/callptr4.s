;
; CC65 runtime: call function via pointer in ptr4
;
; 2018-02-28, Greg King
;

        .export         callptr4
        .importzp       ptr4

callptr4:

; The PC-Engine puts the zero-page at $2000.

        jmp     (ptr4 + $2000)
