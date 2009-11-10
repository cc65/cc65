;
; Atari mode table for tgi_map_mode
;

 	.export	       	_tgi_mode_table

	.include	"tgi-mode.inc"

;----------------------------------------------------------------------------
; Mode table. Contains entries of mode and driver name, the driver name being
; null terminated. A mode with code zero terminates the list. The first entry
; defines also the default mode and driver for the system.
; BEWARE: The current implementation of tgi_map_mode does not work with tables
; larger that 255 bytes!

.rodata

_tgi_mode_table:
        .byte   TGI_MODE_320_192_2, "ATARI8.TGI", 0
        .byte   TGI_MODE_160_192_4, "ATARI15.TGI", 0
        .byte   TGI_MODE_160_192_2, "ATARI14.TGI", 0
        .byte   TGI_MODE_160_96_4, "ATARI7.TGI", 0
        .byte   TGI_MODE_160_96_2, "ATARI6.TGI", 0
        .byte   TGI_MODE_80_48_4, "ATARI5.TGI", 0
        .byte   TGI_MODE_80_48_2, "ATARI4.TGI", 0
        .byte   TGI_MODE_40_24_4, "ATARI3.TGI", 0
        .byte   TGI_MODE_80_192_16b, "ATARI9.TGI", 0
        .byte   TGI_MODE_80_192_9, "ATARI10.TGI", 0
        .byte   TGI_MODE_80_192_16h, "ATARI11.TGI", 0
	; Double paged drivers
        .byte   TGI_MODE_320_192_2_2p, "ATR8P2.TGI", 0
        .byte   TGI_MODE_80_192_16b_2p, "ATR9P2.TGI", 0
        .byte   TGI_MODE_160_192_4_2p, "ATR15P2.TGI", 0
        .byte   TGI_MODE_80_192_9_2p, "ATR10P2.TGI", 0

        .byte   0       ; End marker


