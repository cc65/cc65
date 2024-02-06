; Cartridge "header"
; (In fact, it's at the end of the cartridge, so more a "trailer".)
;
; Christian Groessler, 06-Jan-2014

.ifndef __ATARIXL__

.export         __CART_HEADER__: absolute = 1

.import         __CARTSIZE__, __CARTFLAGS__, cartinit, cartstart

.include        "atari.inc"

.segment        "CARTHDR"

                .word   cartstart       ; start routine
                .byte   0               ; must be zero
                .byte   <__CARTFLAGS__
                .word   cartinit        ; init routine

.assert         (__CARTSIZE__ = $2000 || __CARTSIZE__ = $4000), error, "Cartridge size must either be $2000 or $4000"

.endif  ; .ifndef __ATARIXL__
