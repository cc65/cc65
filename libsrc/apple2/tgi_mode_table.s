;
; Ullrich von Bassewitz, 31.05.2002
;
; Apple II mode table for tgi_map_mode
;

	.export		_tgi_mode_table

	.include	"tgi-mode.inc"

;----------------------------------------------------------------------------
; Mode table. Contains entries of mode and driver name, the driver name being
; null terminated. A mode with code zero terminates the list.
; BEWARE: The current implementation of tgi_map_mode does not work with tables
; larger that 255 bytes!

.rodata

_tgi_mode_table:
	.byte   TGI_MODE_280_192_6, "A2-HI.TGI", 0
	.byte   TGI_MODE_40_40_16,  "A2-LO.TGI", 0
;	.byte   TGI_MODE_560_192_2, "A2-DHI.TGI", 0
	.byte   0       ; End marker
