;
; Oliver Schmidt, 2013-05-16
;

        .constructor    initmainargs, 24
        .import         __argc, __argv, args

        .segment        "ONCE"

initmainargs:
        lda     #<__argv
        ldx     #>__argv
        jsr     args
        sta     __argc
        stx     __argc+1
        rts
