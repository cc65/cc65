;
; CC65 runtime: call function via pointer in ptr4
;

        .export         callptr4
        .importzp       ptr4

callptr4:
        jmp     (ptr4)          ; jump there

