;
; Christian Groessler, May-2016
;
; unsigned char _is_cmdline_dos(void);
;
; returns 0 for non-commandline DOS, 1 for commandline DOS
;

        .export  __is_cmdline_dos
        .import  _doesclrscrafterexit

__is_cmdline_dos:
        jsr     _doesclrscrafterexit    ; currently (unless a DOS behaving differently is popping up)
        eor     #$01                    ; we can get by with the inverse of _doesclrscrafterexit
        rts
