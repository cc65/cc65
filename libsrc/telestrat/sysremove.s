;
; Jede, 10.11.2017
;
; unsigned char __fastcall__ _sysremove (const char* name);
;

        .export         __sysremove


        .include        "zeropage.inc"
        .include        "telestrat.inc"

__sysremove:
        ; Push name
        BRK_TELEMON(XRM)
        rts
