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

_init:    LDX     #$FF                 ; Initialize stack pointer to $01FF
          TXS
          CLD                          ; Clear decimal mode

; ---------------------------------------------------------------------------
; Initialize screen width
; TODO: Can initialization be done in a more idiomatic way?
; TODO: Create function for changing screen width
          LDA     #$18
          STA     SCR_LINELEN

; ---------------------------------------------------------------------------
; Set cc65 argument stack pointer

          LDA     #<(__RAM_START__ + __RAM_SIZE__)
          STA     sp
          LDA     #>(__RAM_START__ + __RAM_SIZE__)
          STA     sp+1

; ---------------------------------------------------------------------------
; Initialize memory storage
; copydata seems to be only necessary for special systems

          JSR     zerobss              ; Clear BSS segment
          ; JSR     copydata           ; Initialize DATA segment
          JSR     initlib              ; Run constructors

; ---------------------------------------------------------------------------
; Call main()

          JSR     _main

; ---------------------------------------------------------------------------
; Back from main (this is also the _exit entry):  force a software break

_exit:    JSR     donelib              ; Run destructors
          BRK

