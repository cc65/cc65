;
; Oliver Schmidt, 2013-05-16
;
; Startup code for cc65 (sim6502 version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .import         zerobss, callmain
        .import         initlib, donelib
        .import         exit
        .import         __RAM_START__, __RAM_SIZE__     ; Linker generated
        .import         __STACKSIZE__                   ; Linker generated

        .include        "zeropage.inc"

        .segment        "STARTUP"

        cld
        ldx     #$FF
        txs
        lda     #<(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
        ldx     #>(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
        sta     sp
        stx     sp+1
        jsr     zerobss
        jsr     initlib
        jsr     callmain
_exit:  pha
        jsr     donelib
        pla
        jmp     exit
