;
; Oliver Schmidt, 24.03.2005
;
; unsigned char __fastcall__ dio_close (dhandle_t handle);
;

        .export         _dio_close
        .import         dioepilog

_dio_close:
        lda     #$00
        jmp     dioepilog
