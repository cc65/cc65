;
; Oliver Schmidt, 2009-09-15
;
; Startup code for cc65 (Apple2 version)
;

        .export         _exit, done, return
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib
        .import         zerobss, callmain
        .import         __ONCE_LOAD__, __ONCE_SIZE__    ; Linker generated
        .import         __LC_START__, __LC_LAST__       ; Linker generated

        .include        "zeropage.inc"
        .include        "apple2.inc"

; ------------------------------------------------------------------------

        .segment        "STARTUP"

        ; ProDOS TechRefMan, chapter 5.2.1:
        ; "For maximum interrupt efficiency, a system program should not
        ;  use more than the upper 3/4 of the stack."
        ldx     #$FF
        txs                     ; Init stack pointer

        ; Save space by putting some of the start-up code in the ONCE segment,
        ; which can be re-used by the BSS segment, the heap and the C stack.
        jsr     init

        ; Clear the BSS data.
        jsr     zerobss

        ; Push the command-line arguments; and, call main().
        jsr     callmain

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

; ------------------------------------------------------------------------

        .segment        "ONCE"

        ; Save the zero-page locations that we need.
init:   ldx     #zpspace-1
:       lda     sp,x
        sta     zpsave,x
        dex
        bpl     :-

        ; Save the original RESET vector.
        ldx     #$02
:       lda     SOFTEV,x
        sta     rvsave,x
        dex
        bpl     :-

        ; Check for ProDOS.
        ldy     $BF00           ; MLI call entry point
        cpy     #$4C            ; Is MLI present? (JMP opcode)
        bne     basic

        ; Check the ProDOS system bit map.
        lda     $BF6F           ; Protection for pages $B8 - $BF
        cmp     #%00000001      ; Exactly system global page is protected
        bne     basic

        ; No BASIC.SYSTEM; so, quit to the ProDOS dispatcher instead.
        lda     #<quit
        ldx     #>quit
        sta     done+1
        stx     done+2

        ; No BASIC.SYSTEM; so, use the addr of the ProDOS system global page.
        lda     #<$BF00
        ldx     #>$BF00
        bne     :+              ; Branch always

        ; Get the highest available mem addr from the BASIC interpreter.
basic:  lda     HIMEM
        ldx     HIMEM+1

        ; Set up the C stack.
:       sta     sp
        stx     sp+1

        ; ProDOS TechRefMan, chapter 5.3.5:
        ; "Your system program should place in the RESET vector the
        ;  address of a routine that ... closes the files."
        ldx     #<_exit
        lda     #>_exit
        jsr     reset           ; Setup RESET vector

        ; Call the module constructors.
        jsr     initlib

        ; Switch in LC bank 2 for W/O.
        bit     $C081
        bit     $C081

        ; Set the source start address.
        ; Aka __LC_LOAD__ iff segment LC exists.
        lda     #<(__ONCE_LOAD__ + __ONCE_SIZE__)
        ldy     #>(__ONCE_LOAD__ + __ONCE_SIZE__)
        sta     $9B
        sty     $9C

        ; Set the source last address.
        ; Aka __LC_LOAD__ + __LC_SIZE__ iff segment LC exists.
        lda     #<((__ONCE_LOAD__ + __ONCE_SIZE__) + (__LC_LAST__ - __LC_START__))
        ldy     #>((__ONCE_LOAD__ + __ONCE_SIZE__) + (__LC_LAST__ - __LC_START__))
        sta     $96
        sty     $97

        ; Set the destination last address.
        ; Aka __LC_RUN__ + __LC_SIZE__ iff segment LC exists.
        lda     #<__LC_LAST__
        ldy     #>__LC_LAST__
        sta     $94
        sty     $95

        ; Call into Applesoft Block Transfer Up -- which handles zero-
        ; sized blocks well -- to move the content of the LC memory area.
        jsr     $D39A           ; BLTU2

        ; Switch in LC bank 2 for R/O and return.
        bit     $C080
        rts

; ------------------------------------------------------------------------

        .code

        ; Set up the RESET vector.
reset:  stx     SOFTEV
        sta     SOFTEV+1
        eor     #$A5
        sta     PWREDUP
return: rts

        ; Quit to the ProDOS dispatcher.
quit:   jsr     $BF00           ; MLI call entry point
        .byte   $65             ; Quit
        .word   q_param

; ------------------------------------------------------------------------

        .rodata

        ; MLI parameter list for quit
q_param:.byte   $04             ; param_count
        .byte   $00             ; quit_type
        .word   $0000           ; reserved
        .byte   $00             ; reserved
        .word   $0000           ; reserved

; ------------------------------------------------------------------------

        .data

        ; Final jump when we're done
done:   jmp     DOSWARM         ; Potentially patched at runtime

; ------------------------------------------------------------------------

        .segment        "INIT"

zpsave: .res    zpspace
rvsave: .res    3
