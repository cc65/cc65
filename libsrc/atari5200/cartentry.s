; Cartridge entry point
;
; Christian Groessler, 01-Mar-2014

.export         __CART_ENTRY__: absolute = 1
.import         __CARTSIZE__, start
.forceimport    __CART_YEAR__, __CART_NAME__

.segment        "CARTENTRY"

                .word   start       ; entry point

.assert         (__CARTSIZE__ = $4000 || __CARTSIZE__ = $8000), error, "Cartridge size must either be $4000 or $8000"
