;
; Christian Groessler, June-2016
;
; unsigned char doesclrscr(void);
;
; returns 0/1 if after program termination the screen isn't/is cleared
;

        .export  _doesclrscrafterexit
        .import  __dos_type
        .include "atari.inc"

_doesclrscrafterexit:
        ldx     #0
        lda     __dos_type
        cmp     #MAX_DOS_WITH_CMDLINE + 1
        txa
        rol     a
        rts
