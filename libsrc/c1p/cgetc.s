;
; char cgetc (void);
;

        .export         _cgetc
        .include        "c1p.inc"

; Use INPUT routine from 65V PROM MONITOR
_cgetc:
        jsr $FEED
        rts
