;
; Ullrich von Bassewitz, 2003-03-07
; Robert Justice, 2026
;
; Push arguments and call main()
;


        .export         callmain, _exit
        .export         __argc, __argv

        .import         _main, pushax, done, donelib

        .include        "zeropage.inc"
        .include        "apple3.inc"


;---------------------------------------------------------------------------
; Setup the stack for main(), then jump to it

callmain:
        lda     __argc
        ldx     __argc+1
        jsr     pushax          ; Push argc

        lda     __argv
        ldx     __argv+1
        jsr     pushax          ; Push argv

        ldy     #4              ; Argument size
        jsr     _main

        ; Avoid a re-entrance of donelib. This is also the exit() entry.
_exit: 
        ; Call the module destructors.
        jsr     donelib

        ; We're done
        jmp     done

;---------------------------------------------------------------------------
; Data

.data
__argc:         .word   0
__argv:         .addr   0
