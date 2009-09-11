;
; Name of the standard tgi driver
;
; Ullrich von Bassewitz, 2009-09-11
;
; const char tgi_stddrv[];
;
                    
        .import _tgi_mode_table
	.export	_tgi_stddrv     = _tgi_mode_table + 1

