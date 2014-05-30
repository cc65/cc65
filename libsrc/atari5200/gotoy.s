;
; Christian Groessler, 13-Mar-2014
;
; void gotoy (unsigned char y);
;

        .include        "atari5200.inc"
        .export         _gotoy
        .import         setcursor

_gotoy:
        sta     ROWCRS_5200     ; Set Y
        jmp     setcursor
