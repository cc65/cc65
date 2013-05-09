;
; Oliver Schmidt, 31.03.2005
;
; unsigned __fastcall__ dio_query_sectsize (dhandle_t handle);
;

        .export         _dio_query_sectsize

        .include        "errno.inc"

_dio_query_sectsize:
        ; Clear error
        stx     __oserror       ; X = 0

        ; Return ProDOS 8 block size
        txa                     ; X = 0
        ldx     #>512
        rts
