; ---------------------------------------------------------------------------
; crt0.s
; ---------------------------------------------------------------------------
;
; Startup code for Ohio Scientific Challenger 1P

.export   _init, _exit
.import   _main

.export   __STARTUP__ : absolute = 1    ; Mark as startup
.import   __MAIN_START__, __MAIN_SIZE__ ; Linker generated
.import   __STACKSIZE__

.import    zerobss, initlib, donelib

.include  "zeropage.inc"
.include  "extzp.inc"
.include  "osic1p.inc"

; ---------------------------------------------------------------------------
; Place the startup code in a special segment

.segment  "STARTUP"

; ---------------------------------------------------------------------------
; A little light 6502 housekeeping

_init:    ldx     #$FF          ; Initialize stack pointer to $01FF
          txs
          cld                   ; Clear decimal mode

; ---------------------------------------------------------------------------
; Set cc65 argument stack pointer

          lda     #<(__MAIN_START__ + __MAIN_SIZE__  + __STACKSIZE__)
          ldx     #>(__MAIN_START__ + __MAIN_SIZE__  + __STACKSIZE__)
          sta     sp
          stx     sp+1

; ---------------------------------------------------------------------------
; Initialize memory storage

          jsr     zerobss       ; Clear BSS segment
          jsr     initlib       ; Run constructors

; ---------------------------------------------------------------------------
; Call main()

          jsr     _main

; ---------------------------------------------------------------------------
; Back from main (this is also the _exit entry):

_exit:    jsr     donelib       ; Run destructors
          jmp     RESET         ; Display boot menu after program exit
