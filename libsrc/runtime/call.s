;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: call function via pointer in ax
;

        .export         callax
        .importzp       ptr1

callax: sta     ptr1
        stx     ptr1+1
        jmp     (ptr1)          ; jump there

