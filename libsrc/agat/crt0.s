;
; Startup code for cc65 (Agat version)
;

        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .export         _exit

        .import         initlib, donelib
        .import         zerobss, callmain
        .import         __ONCE_LOAD__, __ONCE_SIZE__    ; Linker generated

        .include        "zeropage.inc"
        .include        "agat.inc"

; ------------------------------------------------------------------------

    .segment        "STARTUP"
    jsr    init
    jsr    zerobss
    jsr    callmain
_exit:
    ldx    #<exit
    lda    #>exit
    jsr    reset
    jsr    donelib
exit:
    ldx    #$02
:   lda    rvsave,x
    sta    SOFTEV,x
    dex
    bpl    :-
    ldx    #zpspace-1
:   lda    zpsave,x
    sta    c_sp,x
    dex
    bpl    :-
    ldx    #$FF
    txs
    jmp    DOSWARM



    .segment    "ONCE"

init:
    ldx    #zpspace-1
:   lda    c_sp,x
    sta    zpsave,x
    dex
    bpl    :-

    ldx    #$02
:   lda    SOFTEV,x
    sta    rvsave,x
    dex
    bpl    :-

    lda    HIMEM
    ldx    HIMEM+1
    sta    c_sp
    stx    c_sp+1
    ldx    #<_exit
    lda    #>_exit
    jsr    reset
    jmp    initlib

    .code

reset:
    stx    SOFTEV
    sta    SOFTEV+1
    eor    #$A5
    sta    PWREDUP
    rts

    .segment    "INIT"
zpsave:    .res    zpspace
rvsave:    .res    3
