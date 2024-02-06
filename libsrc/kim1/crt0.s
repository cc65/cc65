;
; Startup code for cc65 (KIM-1 version)
;

        .export         _init, _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         _main
        .import         initlib, donelib, copydata, zerobss
        .import         __RAM_START__, __RAM_SIZE__     ; Linker generated
        .import         __STACKSIZE__                   ; Linker generated

        .include        "zeropage.inc"
        .include        "kim1.inc"


; Place the startup code in a special segment

.segment  "STARTUP"


; A little light housekeeping

_init:    cld                          ; Clear decimal mode

; Set cc65 argument stack pointer

          lda     #<(__RAM_START__ + __RAM_SIZE__)
          sta     sp
          lda     #>(__RAM_START__ + __RAM_SIZE__)
          sta     sp+1

; Initialize memory storage

          jsr     zerobss              ; Clear BSS segment
          jsr     copydata             ; Initialize DATA segment
          jsr     initlib              ; Run constructors

; Call main()

          jsr     _main

; Back from main (this is also the _exit entry).  Jumps to the KIM-1 monitor.

_exit:    jmp   START
