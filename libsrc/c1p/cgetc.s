;
; char cgetc (void);
;
        .export         _cgetc
        .include        "c1p.inc"

; Direct use of input routine from 65V PROM MONITOR
_cgetc = INPUTC
