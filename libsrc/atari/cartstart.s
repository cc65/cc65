; Cartridge start routine
;
; Christian Groessler, 06-Jan-2014

.ifndef __ATARIXL__

.export         cartstart

.import         start, copydata

.include        "atari.inc"

; start routine of cartridge
; copy data segment to RAM and chain to entry point of crt0.s

cartstart:      jsr     copydata
                jsr     start                   ; run program
                jmp     (DOSVEC)                ; return to DOS

.endif  ; .ifndef __ATARIXL__
