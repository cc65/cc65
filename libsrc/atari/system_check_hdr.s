;
; Atari startup system check headers
;
; Christian Groessler, chris@groessler.org, 2013
;
        .export         __SYSCHKHDR__: absolute = 1
        .import         __SYSCHK_LOAD__, __SYSCHK_END__

; ------------------------------------------------------------------------
; Chunk header

.segment        "SYSCHKHDR"

        .word   __SYSCHK_LOAD__
        .word   __SYSCHK_END__ - 1

