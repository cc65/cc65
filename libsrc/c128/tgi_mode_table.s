;
; Ullrich von Bassewitz, 03.10.2002
;
; C128 mode table for tgi_map_mode
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
        .byte   TGI_MODE_320_200_2, "c128-320-200-2.tgi", 0
        .byte   0       ; End marker


