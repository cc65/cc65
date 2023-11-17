;
; 2023, Rumbledethumps
;
; crt0.s

.export _init, _exit
.import _main

.export __STARTUP__ : absolute = 1
.import __RAM_START__, __RAM_SIZE__

.import copydata, zerobss, initlib, donelib

.include "rp6502.inc"
.include "zeropage.inc"

.segment  "STARTUP"

; Essential 6502 startup the CPU doesn't do
_init:
    ldx #$FF
    txs
    cld

; Set cc65 argument stack pointer
    lda #<(__RAM_START__ + __RAM_SIZE__)
    sta sp
    lda #>(__RAM_START__ + __RAM_SIZE__)
    sta sp+1

; Initialize memory storage
    jsr zerobss   ; Clear BSS segment
    jsr copydata  ; Initialize DATA segment
    jsr initlib   ; Run constructors

; Call main()
    jsr _main

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
    lda #$FF     ; exit()
    sta RIA_OP
    stp
