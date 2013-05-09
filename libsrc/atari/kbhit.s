;
; Christian Groessler, 19-Feb-2000
;
; unsigned char kbhit (void);
;

        .export         _kbhit
        .import         return1

        .include        "atari.inc"

.proc   _kbhit

        ldx     CH              ; last pressed key
        inx                     ; 255 means "no key"
        bne     L1
        txa                     ; X = A = 0
        rts
L1:     jmp     return1

.endproc


