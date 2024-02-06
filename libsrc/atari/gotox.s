;
; Christian Groessler, 19-Feb-2000
;
; void gotox (unsigned char x);
;

        .include        "atari.inc"
        .export         _gotox
        .import         setcursor

_gotox:
        sta     COLCRS          ; Set X
        lda     #0
        sta     COLCRS+1
        jmp     setcursor
