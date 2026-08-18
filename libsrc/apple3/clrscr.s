;
; Robert Justice 2026
;
; void clrscr (void);
;

        .export         _clrscr
        .import         putcdirect

        .include        "apple3.inc"

_clrscr:
        lda     #CONSOLE_CLR_VIEWPORT
        jmp     putcdirect  

