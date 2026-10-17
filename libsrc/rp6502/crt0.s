;
; 2023, Rumbledethumps
;
; crt0.s

.export _exit
.import callmain

.export __STARTUP__ : absolute = 1
.import __RAM_START__, __RAM_SIZE__, __STACKSIZE__

.import zerobss, initlib, donelib

; BSS overlays ONCE, so it is cleared after the constructors in ONCE have run.
; cc65 libraries use priority 7 and up.
.constructor clearbss, 6
clearbss := zerobss

.include "rp6502.inc"
.include "zeropage.inc"

.segment  "STARTUP"

; Essential 6502 startup the CPU doesn't do
    ldx #$FF
    txs

    jsr init

; Call main()
    jsr callmain

; Back from main() also the _exit entry
; Stack the exit value in case destructors call OS
_exit:
    phx
    pha
    jsr donelib  ; Run destructors
    pla
    sta RIA_A
    plx
    stx RIA_X
    lda #RIA_OP_EXIT
    sta RIA_OP

.segment  "ONCE"

; Set cc65 argument stack pointer
init:
    lda #<(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
    sta c_sp
    lda #>(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
    sta c_sp+1

; Run constructors
    jmp initlib
