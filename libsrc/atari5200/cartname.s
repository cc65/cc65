; default cartridge name
;
; Christian Groessler, 01-Mar-2014

.include	"atari5200.mac"

.export         __CART_NAME__: absolute = 1

.segment        "CARTNAME"

		scrcode "   cc65 compiled"
