;
; Christian Groessler, October 2000
;
; this file provides the _dio_read function
;
; unsigned char __fastcall__ dio_read(dhandle_t handle,unsigned sect_num,void *buffer);
; dhandle_t - 16bit (ptr)
;

        .import         __sio_call,pushax
        .export         _dio_read
        .include        "atari.inc"

.proc   _dio_read

        jsr     pushax          ; push buffer address
        ldx     #%01000000      ; direction value
        lda     #SIO_READ       ; read sector command
        jmp     __sio_call      ; do the call and return to the user

.endproc

