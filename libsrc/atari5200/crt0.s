;
; Startup code for cc65 (Atari5200 version)
;
; Christian Groessler (chris@groessler.org), 2014
;

        .export         _exit, start
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .import         __RAM_START__, __RAM_SIZE__
        .import         __RESERVED_MEMORY__

        .import         initlib, donelib, callmain
        .import         zerobss, copydata

        .include        "zeropage.inc"
        .include        "atari5200.inc"

start:

; Clear the BSS data

        jsr     zerobss

; initialize data
        jsr     copydata

; setup the stack

        lda     #<(__RAM_START__ + __RAM_SIZE__ - __RESERVED_MEMORY__)
        sta     sp
        lda     #>(__RAM_START__ + __RAM_SIZE__ - __RESERVED_MEMORY__)
        sta     sp+1            ; Set argument stack ptr

; Call module constructors

        jsr     initlib

; Push arguments and call main()

        jsr     callmain

; Call module destructors. This is also the _exit entry.

_exit:  jsr     donelib         ; Run module destructors

; A 5200 program isn't supposed to exit.

halt:   jmp halt
