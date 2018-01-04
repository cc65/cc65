       .export         _exit
       .export         __STARTUP__ : absolute = 1      ; Mark as startup
       .import         zerobss, _main
       .import         initlib, donelib
       .import         __MAIN_START__, __MAIN_SIZE__   ; Linker generated
       .import         __STACKSIZE__                   ; Linker generated

       .include "zeropage.inc"

       .segment "STARTUP"

       cld
       ldx #$FF
       txs
       lda #<(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
       ldx #<(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
       sta sp
       stx sp+1
       jsr zerobss
       jsr initlib
       jsr _main
_exit: pha
       jsr donelib
       pla
       brk
