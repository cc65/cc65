;
; Oliver Schmidt, 31.03.2005
;
; sectnum_t __fastcall__ dio_query_sectcount (dhandle_t handle);
;

        .export 	_dio_query_sectcount

_dio_query_sectcount:
        lda	#<280
        ldx	#>280
        rts
