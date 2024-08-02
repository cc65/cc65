;
; void clrscr (void);
;

        .export         _clrscr
        .include        "atari.inc"
        .import         _bzero
        .import         pushax
        .import         setcursor

SCRSIZE =       960             ; 40x24: size of default atari screen

_clrscr:
        lda     SAVMSC          ; screen memory
        ldx     SAVMSC+1

        jsr     pushax          ; pointer to memory to clear

        lda     #<SCRSIZE
        ldx     #>SCRSIZE

        jsr     _bzero

        lda     #0
        sta     OLDCHR
        sta     COLCRS
        sta     ROWCRS

        jmp     setcursor
