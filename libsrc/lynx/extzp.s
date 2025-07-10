;
; Ullrich von Bassewitz, 2004-11-06
;
; zeropage locations for exclusive use by the library
;

        .include "extzp.inc"

        .segment "EXTZP" : zeropage

; ------------------------------------------------------------------------
; mikey and suzy shadow registers

__iodat:    .res    1
__iodir:    .res    1
__viddma:   .res    1
__sprsys:   .res    1

; ------------------------------------------------------------------------
; Filesystem variables needed for reading stuff from the Lynx cart
_FileEntry:                     ; The file directory entry is 8 bytes
_FileStartBlock:    .res   1
_FileBlockOffset:   .res   2
_FileExecFlag:      .res   1
_FileDestAddr:      .res   2
_FileFileLen:       .res   2

_FileCurrBlock:     .res   1
_FileBlockByte:     .res   2
_FileDestPtr:       .res   2
