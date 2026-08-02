;
; Robert Justice, 2026
;
; void waitvsync (void);
;
        .export         _waitvsync
        .import         _cputc

        .include        "apple3.inc"

_waitvsync:
        lda     #CONSOLE_SCREEN_SYNC
        jmp     _cputc  
