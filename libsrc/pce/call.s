;
; CC65 runtime: call function via pointer in ax
;
; 1998-08-06, Ullrich von Bassewitz
; 2018-02-28, Greg King
;

        .export         callax
        .importzp       ptr1

callax: sta     ptr1
        stx     ptr1+1

; The PC-Engine puts the zero-page at $2000.

        jmp     (ptr1 + $2000)  ; go there
