; ***
; CC65 Lynx Library
;
; Originally by Bastian Schick
; http://www.geocities.com/SiliconValley/Byte/4242/lynx/
;
; Ported to cc65 (http://www.cc65.org) by
; Shawn Jefferson, June 2004
;
; This version by Karri Kaksonen, December 2010
;
; Helper stuff for the cartridge file functions. This version can deal
; with 1024 bytes/block carts that are using CART0 as a read strobe.
; Also the default crt0.s supports this most common Lynx cart format.

        .include "lynx.inc"
        .include "extzp.inc"
        .export  lynxskip0, lynxread0
        .export  lynxblock
        .import  __BANK0BLOCKSIZE__

        .code

;**********************************
; Skip bytes on bank 0
; X:Y count (EOR $FFFF)
;**********************************
lynxskip0:
        inx
        bne @0
        iny
        beq exit
@0:     jsr readbyte0
        bra lynxskip0

;**********************************
; Read bytes from bank 0
; X:Y count (EOR $ffff)
;**********************************
lynxread0:
        inx
        bne @1
        iny
        beq exit
@1:     jsr readbyte0
        sta (_FileDestPtr)
        inc _FileDestPtr
        bne lynxread0
        inc _FileDestPtr+1
        bra lynxread0

;**********************************
; Read one byte from cartridge
;**********************************
readbyte0:
        lda RCART0
        inc _FileBlockByte
        bne exit
        inc _FileBlockByte+1
        bne exit

;**********************************
; Select a block
;**********************************
lynxblock:
        pha
        phx
        phy
        lda __iodat
        and #$fc
        tay
        ora #2
        tax
        lda _FileCurrBlock
        inc _FileCurrBlock
        sec
        bra @2
@0:     bcc @1
        stx IODAT
        clc
@1:     inx
        stx SYSCTL1
        dex
@2:     stx SYSCTL1
        rol
        sty IODAT
        bne @0
        lda __iodat
        sta IODAT
        stz _FileBlockByte
        lda #<($100-(>__BANK0BLOCKSIZE__))
        sta _FileBlockByte+1
        ply
        plx
        pla

exit:   rts

