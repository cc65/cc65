;
; Oliver Schmidt, 31.03.2005
;
; sectsize_t __fastcall__ dio_query_sectsize(dhandle_t handle);
;

        .export 	_dio_query_sectsize

_dio_query_sectsize:
        lda	#<512
        ldx	#>512
        rts
