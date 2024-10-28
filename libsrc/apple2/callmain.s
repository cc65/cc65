;
; Ullrich von Bassewitz, 2003-03-07
;
; Push arguments and call main()
;


        .export         callmain, _exit
        .export         __argc, __argv

        .import         _main, pushax, done, donelib
        .import         zpsave, rvsave, reset

        .include        "zeropage.inc"
        .include        "apple2.inc"


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
_exit:  ldx     #<exit
        lda     #>exit
        jsr     reset           ; Setup RESET vector

        ; Switch in LC bank 2 for R/O in case it was switched out by a RESET.
        bit     $C080

        ; Call the module destructors.
        jsr     donelib

        ; Switch in ROM.
        bit     $C082

        ; Restore the original RESET vector.
exit:   ldx     #$02
:       lda     rvsave,x
        sta     SOFTEV,x
        dex
        bpl     :-

        ; Copy back the zero-page stuff.
        ldx     #zpspace-1
:       lda     zpsave,x
        sta     sp,x
        dex
        bpl     :-

        ; ProDOS TechRefMan, chapter 5.2.1:
        ; "System programs should set the stack pointer to $FF at the
        ;  warm-start entry point."
        ldx     #$FF
        txs                     ; Re-init stack pointer

        ; We're done
        jmp     done

;---------------------------------------------------------------------------
; Data

.data
__argc:         .word   0
__argv:         .addr   0
