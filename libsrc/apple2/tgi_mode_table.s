;
; Ullrich von Bassewitz, 2003-11-12
;
; Apple ][ mode table for tgi_map_mode
;

 	.export	       	_tgi_mode_table

	.include	"tgi-mode.inc"

;----------------------------------------------------------------------------
; Mode table. Contains entries of mode and driver name, the driver name being
; null terminated. A mode with code zero terminates the list.
; BEWARE: The current implementation of tgi_map_mode does not work with tables
; larger that 255 bytes!

.rodata

_tgi_mode_table:
        .byte   TGI_MODE_280_192_6, "apple2-280-192-6.tgi", 0
        .byte   0       ; End marker


