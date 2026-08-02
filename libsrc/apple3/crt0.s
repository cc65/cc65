;
; Oliver Schmidt, 2009-09-15
; Robert Justice, 2026
;
; Startup code for cc65 (Apple3 version)
;

        .export         return, done
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib
        .import         zerobss, callmain
        .import         __ONCE_LOAD__, __ONCE_SIZE__    ; Linker generated

        .include        "zeropage.inc"
        .include        "apple3.inc"

; ------------------------------------------------------------------------

        .segment        "STARTUP"

        ldx     #$FF
        txs                     ; Init stack pointer

        ; Save space by putting some of the start-up code in the ONCE segment,
        ; which can be re-used by the BSS segment, the heap and the C stack.
        jsr     init

        ; Clear the BSS data.
        jsr     zerobss

        ; Push the command-line arguments; and, call main().
        jsr     callmain

; ------------------------------------------------------------------------

        .segment        "ONCE"

init:
        ; zero interpreter extended addressing page to disable
        ldx     #0
        txa
:       sta     $1600,x
        inx
        bne     :-

        ; quit to SOS
        lda     #<quit
        ldx     #>quit
        sta     done+1
        stx     done+2

        ; use the addr of the top of the Interpreter space
        lda     #<$B7FF
        ldx     #>$B7FF

        ; Set up the C stack.
        sta     c_sp
        stx     c_sp+1

        ; Call the module constructors.
        jsr     initlib

        rts

; ------------------------------------------------------------------------

        .code

return: rts

        ; Quit to SOS
quit:   brk                     ; SOS Terminate
        .byte   $65             ; Quit
        .word   quit            ; points to param count of zero
		
; ------------------------------------------------------------------------

        .data

        ; Final jump when we're done
done:   jmp     quit            ; Potentially patched at runtime

