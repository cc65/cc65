; Cartridge copyright year
;
; Christian Groessler, 01-Mar-2014

.export         __CART_YEAR__: absolute = 1

.segment        "CARTYEAR"

                .byte   '9' + 32,'8' + 32       ; "98", but using playfield 1
