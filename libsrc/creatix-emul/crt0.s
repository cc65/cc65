; crt0.s

        .setcpu "c39-native"
        
        .export         __STARTUP__ : absolute = 1
        
        .export         _startup, _exit
        
        .import         copydata, zerobss, initlib, donelib
        .import         callmain
        
        .import         __RAM_START__, __RAM_SIZE__, __BSS_LOAD__

        .macpack        cpu
        
        .include        "zeropage.inc"

.segment "STARTUP"

_startup:
        lda     #<(__RAM_START__ + __RAM_SIZE__)
        sta     sp
        lda     #>(__RAM_START__ + __RAM_SIZE__)
        sta     sp+1            

        jsr     zerobss
        jsr     copydata
        jsr     initlib
        jsr     callmain

_exit:
        jsr     donelib
        brk
