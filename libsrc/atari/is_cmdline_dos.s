;
; Christian Groessler, May-2016
;
; unsigned char _is_cmdline_dos(void);
;
; returns 0 for non-commandline DOS, 1 for commandline DOS
;

        .export  __is_cmdline_dos
        .import  __dos_type
        .include "atari.inc"

__is_cmdline_dos:
        lda     #MAX_DOS_WITH_CMDLINE
        cmp     __dos_type
        lda     #0
        rol     a
        rts
