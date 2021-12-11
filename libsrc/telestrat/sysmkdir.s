;
; Jede (jede@oric.org), 2017-10-27
;
; unsigned char _sysmkdir (const char* name, ...);
;

        .export         __sysmkdir
        .import         addysp, popax

        .include        "telestrat.inc"
        .include        "zeropage.inc"
        

__sysmkdir:
        ; Throw away all parameters except the name
        dey
        dey
        jsr     addysp

        ; Get name
        jsr     popax
        
        ; Call telemon primitive
        
        BRK_TELEMON(XMKDIR)
        
        rts



