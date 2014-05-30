;
; Christian Groessler, 13-Mar-2014
;
; void gotox (unsigned char x);
;

        .include        "atari5200.inc"
        .export         _gotox
        .import         setcursor

_gotox:
        sta     COLCRS_5200     ; Set X
        jmp     setcursor
