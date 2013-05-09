;
; Maciej 'YTM/Elysium' Witkowiak
; 2.7.2001
;
; this file provides the _dio_write function
;
; unsigned char __fastcall__ dio_write_verify (dhandle_t handle, unsigned sect_num, const void *buffer);
;

            .export _dio_write_verify
            .import dio_params, __oserror

            .include "geossym.inc"
            .include "jumptab.inc"

_dio_write_verify:
        jsr dio_params
        jsr VerWriteBlock
        stx __oserror
        txa
        rts
