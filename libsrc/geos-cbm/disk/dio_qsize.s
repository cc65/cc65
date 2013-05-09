;
; Oliver Schmidt, 2012-10-23
;
; unsigned __fastcall__ dio_query_sectsize (dhandle_t handle);
;

            .export _dio_query_sectsize
            .import __oserror

_dio_query_sectsize:
        lda #<256
        ldx #>256
        sta __oserror
        rts
