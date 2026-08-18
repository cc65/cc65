; mainargs.s
;
; Robert Justice, 2026
;
; Just a dummy, Apple3 did not have any command line support
;

        .constructor    initmainargs, 24
        .import         __argc, __argv

        .include        "zeropage.inc"
        .include        "apple3.inc"


; Goes into the special ONCE segment,
; which may be reused after the startup code is run.

        .segment        "ONCE"

initmainargs:

        rts

