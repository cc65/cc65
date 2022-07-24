;
; 2001-07-02, Maciej 'YTM/Elysium' Witkowiak
; 2015-08-27, Greg King
;
; this file provides the _dio_write function
;
; unsigned char __fastcall__ dio_write (dhandle_t handle, unsigned sect_num, const void *buffer);
;

            .export _dio_write
            .import dio_params, setoserror

            .include "geossym.inc"
            .include "jumptab.inc"

_dio_write:
        jsr dio_params
        tay
        bne err
        jsr WriteBlock
        jmp setoserror

err:    rts
