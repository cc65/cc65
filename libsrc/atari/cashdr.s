;
; Cassette boot file header
;
; Christian Groessler, chris@groessler.org, 2014
;

;DEBUG   = 1

.ifndef __ATARIXL__

        .include "atari.inc"

        .import __BSS_RUN__, __STARTADDRESS__, _cas_init
        .export _cas_hdr

.assert ((__BSS_RUN__ - __STARTADDRESS__ + 127) / 128) < $101, error, "File to big to load from cassette"


; for a description of the cassette header, see De Re Atari, appendix C

.segment        "CASHDR"

_cas_hdr:
        .byte   0                       ; ignored
        .byte   <((__BSS_RUN__ - __STARTADDRESS__ + 127) / 128)         ; # of 128-byte records to read
        .word   __STARTADDRESS__        ; load address
        .word   _cas_init               ; init address

.ifdef  DEBUG
        lda     #33
        ldy     #80
        sta     (SAVMSC),y
.endif
        clc
        rts

.endif  ; .ifdef __ATARIXL__
