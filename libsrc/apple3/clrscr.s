;
; Robert Justice 2026
;
; void clrscr (void);
;

        .export         _clrscr
        .import         _cputc

        .include        "apple3.inc"

_clrscr:
        lda     #CONSOLE_CLR_VIEWPORT
        jmp     _cputc  

