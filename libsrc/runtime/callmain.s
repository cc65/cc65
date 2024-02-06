;
; Ullrich von Bassewitz, 2003-03-07
;
; Push arguments and call main()
;


        .export         callmain
        .export         __argc, __argv

        .import         _main, pushax

;---------------------------------------------------------------------------
; Setup the stack for main(), then jump to it

.proc   callmain

        lda     __argc
        ldx     __argc+1
        jsr     pushax          ; Push argc

        lda     __argv
        ldx     __argv+1
        jsr     pushax          ; Push argv

        ldy     #4              ; Argument size
        jmp     _main

.endproc

;---------------------------------------------------------------------------
; Data

.data
__argc:         .word   0
__argv:         .addr   0




