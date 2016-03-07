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

; Clear the BSS data.

        jsr     zerobss

; Initialize the data.
        jsr     copydata

; Set up the stack.

        lda     #<(__RAM_START__ + __RAM_SIZE__ - __RESERVED_MEMORY__)
        ldx     #>(__RAM_START__ + __RAM_SIZE__ - __RESERVED_MEMORY__)
        sta     sp
        stx     sp+1            ; Set argument stack ptr

; Call the module constructors.

        jsr     initlib

; Push the command-line arguments; and, call main().

        jsr     callmain

; Call the module destructors. This is also the exit() entry.

_exit:  jsr     donelib         ; Run module destructors

; A 5200 program isn't supposed to exit.

halt:   jmp halt
