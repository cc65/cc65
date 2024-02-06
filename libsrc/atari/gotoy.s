;
; Christian Groessler, 19-Feb-2000
;
; void gotoy (unsigned char y);
;

        .include        "atari.inc"
        .export         _gotoy
        .import         setcursor

_gotoy:
        sta     ROWCRS          ; Set Y
        jmp     setcursor
