;
; get_tv.inc
;
; Ullrich von Bassewitz, 2004-10-15
;
; Defines for the get_tv function.



; Error codes returned by all functions
.enum
    TV_NTSC     
    TV_PAL
    TV_OTHER
.endenum


; get_tv function

        .global _get_tv


