;
; Cassette boot file init routine
;
; Christian Groessler, chris@groessler.org, 2014
;

;DEBUG   = 1

.ifndef __ATARIXL__

        .include "atari.inc"

        .import start
        .export _cas_init

.segment        "ONCE"

_cas_init:
.ifdef  DEBUG
        lda     #34
        ldy     #81
        sta     (SAVMSC),y
.endif

        lda     #<start
        sta     DOSVEC
        lda     #>start
        sta     DOSVEC+1
        rts

.endif  ; .ifdef __ATARIXL__
