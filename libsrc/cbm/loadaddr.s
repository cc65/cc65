;
; Ullrich von Bassewitz, 2010-11-13
;
; This module supplies the load address that is expected by Commodore
; machines in the first two bytes of an excutable disk file.
;


        ; The following symbol is used by linker config to force the module
        ; to get included into the output file
        .export         __LOADADDR__: absolute = 1

.segment        "LOADADDR"

        .addr   *+2

