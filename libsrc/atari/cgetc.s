;
; Christian Groessler, November-2002
;
; get a char from the keyboard
; char cgetc(void)
;

        .include "atari.inc"
        .export _cgetc
        .import setcursor
        .import KEYBDV_handler

_cgetc:
        jsr     setcursor
        lda     #12
        sta     ICAX1Z          ; fix problems with direct call to KEYBDV
.ifdef __ATARIXL__
        jsr     KEYBDV_handler
.else
        jsr     @1
.endif
        ldx     #0
        rts

.ifndef __ATARIXL__
@1:     lda     KEYBDV+5
        pha
        lda     KEYBDV+4
        pha
        rts
.endif
