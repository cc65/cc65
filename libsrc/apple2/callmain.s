;
; Ullrich von Bassewitz, 2003-03-07
;
; Push arguments and call main()
;


        .export         callmain, _exit
        .export         exit_with_params
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

_exit:
        ; If we reach that point (rts from main or direct exit() call),
        ; we're not exec()ing anything. Clear this program's possibly
        ; left-over parameters, in order to avoid passing them to the
        ; next program executed via ProDOS quit code.
        ; We do that before _exit as _exec calls _exit, and in
        ; this case, we want to keep the parameters that the user
        ; possibly just set.
.if (.cpu .bitand ::CPU_ISET_65SC02)
        stz     $0100
.else
        lda     #$00
        sta     $0100
.endif

        ; Avoid a re-entrance of donelib.
exit_with_params:
        ldx     #<exit
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
        sta     c_sp,x
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
