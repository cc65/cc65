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
        lda     __dos_type
        cmp     #MAX_DOS_WITH_CMDLINE + 1
        lda     #0
        rol     a
        eor     #$01
        rts
