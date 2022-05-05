;
; Jede (jede@oric.org), 2017-10-27
;
; unsigned char _sysmkdir (const char* name, ...);
;
; This routine only works with Orix


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

        stx     tmp1
        ldy     tmp1

        ldx     #$00         ; X register is used to set if all folders must be created

        ; Call telemon primitive
        
        BRK_TELEMON(XMKDIR)

        rts
