;
; Jede (jede@oric.org), 2021-02-22
;
; unsigned char _syschdir (const char* name, ...);
;

        .export         __syschdir
        .import         addysp, popax
        .importzp       tmp1
        .import         initcwd

        .include        "telestrat.inc"
        .include        "zeropage.inc"


__syschdir:
        ; throw away all parameters except the name
        dey
        dey
        jsr     addysp

        ; get name
        jsr     popax

        stx     tmp1
        ldy     tmp1


        ; Call telemon primitive
        BRK_TELEMON(XPUTCWD)

        jmp     initcwd      ; update cwd
