;
; 2022-03-28, Greg King
;
; unsigned char kbhit (void);
; /* Returns non-zero (true) if a typed character is waiting. */
;

        .export         _kbhit

        .import         KBDBUF_PEEK


.proc   _kbhit
        jsr     KBDBUF_PEEK
        txa                     ; Low byte of return (only its zero/nonzero ...
        rts                     ; ... state matters)
.endproc
