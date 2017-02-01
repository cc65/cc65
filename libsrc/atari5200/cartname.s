; default cartridge name
;
; Christian Groessler, 01-Mar-2014

.export         __CART_NAME__: absolute = 1

.macpack        atari

.segment        "CARTNAME"

                scrcode "   cc"
                .byte   '6' + 32, '5' + 32      ; use playfield 1
                scrcode " compiled"
