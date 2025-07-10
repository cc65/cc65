       .export         _exit
       .export         __STARTUP__ : absolute = 1      ; Mark as startup
       .import         zerobss, _main
       .import         initlib, donelib
       .import         __STACKSTART__                  ; Linker generated

       .include "zeropage.inc"

       .segment "STARTUP"

       lda #<__STACKSTART__
       ldx #>__STACKSTART__
       sta c_sp
       stx c_sp+1
       jsr zerobss
       jsr initlib
       jsr _main
_exit: pha
       jsr donelib
       pla
       rts
