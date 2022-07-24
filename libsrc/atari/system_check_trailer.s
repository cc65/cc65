;
; Atari startup system check headers
;
; Christian Groessler, chris@groessler.org, 2013
;
        .export         __SYSCHKTRL__: absolute = 1
        .import         __SYSTEM_CHECK__

        .include        "atari.inc"
; ------------------------------------------------------------------------
; Chunk "trailer" - sets INITAD

.segment        "SYSCHKTRL"

        .word   INITAD
        .word   INITAD+1
        .word   __SYSTEM_CHECK__
