;
; Oliver Schmidt, 2012-06-10
;
; This module supplies a 4 byte DOS 3.3 header
; containing the load address and load length.
;

        .export         __EXEHDR__ : absolute = 1	; Linker referenced
        .import        	__RAM_START__, __ZPSAVE_RUN__	; Linker generated
        .import         __MOVE_START__, __MOVE_LAST__	; Linker generated

        .linecont	+

; ------------------------------------------------------------------------

        .segment        "EXEHDR"

        .addr           __RAM_START__			; Start address
        .word           __ZPSAVE_RUN__ - __RAM_START__ + \
			__MOVE_LAST__  - __MOVE_START__	; Size
