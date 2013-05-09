;
; Maciej 'YTM/Elysium' Witkowiak
; 2.7.2001
;
; this file provides the _dio_read function
;
; unsigned char __fastcall__ dio_read (dhandle_t handle, unsigned sect_num, void *buffer);
;

            .export _dio_read
            .import dio_params, __oserror

            .include "geossym.inc"
            .include "jumptab.inc"

_dio_read:
        jsr dio_params
        jsr ReadBlock
        stx __oserror
        txa
        rts
