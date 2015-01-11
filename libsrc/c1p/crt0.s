; ---------------------------------------------------------------------------
; crt0.s
; ---------------------------------------------------------------------------
;
; Startup code for Ohio Scientific Challenger 1P

.export   _init, _exit
.import   _main

.export   __STARTUP__ : absolute = 1        ; Mark as startup
.import   __RAM_START__, __RAM_SIZE__       ; Linker generated

.import    zerobss, initlib, donelib

.include  "zeropage.inc"
.include  "extzp.inc"

; ---------------------------------------------------------------------------
; Place the startup code in a special segment

.segment  "STARTUP"

; ---------------------------------------------------------------------------
; A little light 6502 housekeeping

_init:    ldx     #$FF                 ; Initialize stack pointer to $01FF
          txs
          cld                          ; Clear decimal mode

; ---------------------------------------------------------------------------
; Initialize screen width
; TODO: Can initialization be done in a more idiomatic way?
; TODO: Create function for changing screen width
          lda     #$18
          sta     SCR_LINELEN

; ---------------------------------------------------------------------------
; Set cc65 argument stack pointer

          lda     #<(__RAM_START__ + __RAM_SIZE__)
          sta     sp
          lda     #>(__RAM_START__ + __RAM_SIZE__)
          sta     sp+1

; ---------------------------------------------------------------------------
; Initialize memory storage
; copydata seems to be only necessary for special systems

          jsr     zerobss              ; Clear BSS segment
          ; jsr     copydata           ; Initialize DATA segment
          jsr     initlib              ; Run constructors

; ---------------------------------------------------------------------------
; Call main()

          jsr     _main

; ---------------------------------------------------------------------------
; Back from main (this is also the _exit entry):  force a software break

_exit:    jsr     donelib              ; Run destructors
          brk
