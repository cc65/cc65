; test .addrsize and ensure .feature addrsize is allowed, but inactive

.export _main

.segment "ZEROPAGE"
zplabel:

.segment "CODE"
abslabel:

; exit with 0

_main:
    lda #0
    tax
    rts


.assert .addrsize(zplabel) = 1, error, ".addrsize 1 expected for ZEROPAGE"
.assert .addrsize(abslabel) = 2, error, ".addrsize 2 expected for absolute"

.feature addrsize
.assert .addrsize(zplabel) = 1, error, ".addrsize 1 expected for ZEROPAGE"
.assert .addrsize(abslabel) = 2, error, ".addrsize 2 expected for absolute"

.feature addrsize +
.assert .addrsize(zplabel) = 1, error, ".addrsize 1 expected for ZEROPAGE"
.assert .addrsize(abslabel) = 2, error, ".addrsize 2 expected for absolute"

.feature addrsize -
.assert .addrsize(zplabel) = 1, error, ".addrsize 1 expected for ZEROPAGE"
.assert .addrsize(abslabel) = 2, error, ".addrsize 2 expected for absolute"
