; ***
; CC65 Lynx Library
;
; Originally by Bastian Schick
; http://www.geocities.com/SiliconValley/Byte/4242/lynx/
;
; Ported to cc65 (http://www.cc65.org) by
; Shawn Jefferson, June 2004
;
; ***
;
; void __fastcall__ read_cart_block(unsigned char block, char *dest)
;
; Reads an entire block (1024 bytes) from the cartridge into the buffer
; pointed to by dest.


       	.export     _read_cart_block
       	.import     FileSelectBlock, FileReadBytes
       	.import     popa

        .include    "extzp.inc"


.code

;*******************************************
; void read_cart_block (unsigned char block, char *dest);
; loads one Block (1024 bytes) to pDest

_read_cart_block:
       	sta     _FileDestPtr            ;  lo
       	stx     _FileDestPtr+1          ;  hi

       	jsr     popa                    ; bBlock

       	sta     _FileCurrBlock
       	jsr     FileSelectBlock         ; select block# (Accu)

       	lda     #<1024                  ; load a whole block (1024 bytes)
       	eor     #$ff
       	tax
       	lda     #>1024
       	eor     #$ff
       	tay

       	jmp     FileReadBytes


