; default cartridge name
;
; Christian Groessler, 01-Mar-2014

.include        "atari.mac"

.export         __CART_NAME__: absolute = 1

.segment        "CARTNAME"

                scrcode "   cc"
                .byte   '6' + 32, '5' + 32      ; use playfield 1
                scrcode " compiled"
