;
; Oliver Schmidt, 2012-10-23
;
; unsigned __fastcall__ dio_query_sectcount (dhandle_t handle);
;

            .export _dio_query_sectcount
            .import ___oserror

_dio_query_sectcount:
        lda #0
        sta ___oserror
        lda #<683
        ldx #>683
        rts
