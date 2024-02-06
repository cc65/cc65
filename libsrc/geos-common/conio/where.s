;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001
; 06.03.2002
; 02.01.2003

; unsigned char wherex (void);
; unsigned char wherey (void);

            .export _wherex, _wherey
            .importzp cursor_c, cursor_r

_wherex:
        lda cursor_c
        ldx #0
        rts

_wherey:
        lda cursor_r
        ldx #0
        rts
