;
; 2001-07-02, Maciej 'YTM/Elysium' Witkowiak
; 2015-08-27, Greg King
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
        tay
        bne err
        jsr ReadBlock
        stx __oserror
        txa
err:    rts
