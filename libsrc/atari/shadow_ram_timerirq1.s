;
; Atari XL shadow RAM timer IRQ #1 handler
;
; Christian Groessler, chris@groessler.org, 2014
;

;DEBUG           =       1

.ifdef __ATARIXL__

SHRAM_HANDLERS  =       1
                .include        "atari.inc"
                .include        "romswitch.inc"
                .export         set_VTIMR1_handler


.segment "LOWBSS"

VTIMR1_handler: .res    3


.segment "BSS"

old_VTIMR1_handler:
                .res    2


.segment "LOWCODE"

; timer interrupt handler:
; disable ROM, call user handler, enable ROM again

my_VTIMR1_handler:
                disable_rom_quick
                jsr     VTIMR1_handler
                enable_rom_quick
                pla
                rti

.segment "CODE"

; install or remove VTIMR1 handler
; input: CF - 0/1 for remove/install handler
;        AX - pointer to handler (if CF=1)
; registers destroyed

set_VTIMR1_handler:

                bcc     @remove

; install vector

                stx     VTIMR1_handler+2
                sta     VTIMR1_handler+1        ; save passed vector in low memory
                lda     #$4C                    ; "JMP" opcode
                sta     VTIMR1_handler

                lda     VTIMR1
                sta     old_VTIMR1_handler
                lda     VTIMR1+1
                sta     old_VTIMR1_handler+1

                lda     #<my_VTIMR1_handler
                php
                sei
                sta     VTIMR1
                lda     #>my_VTIMR1_handler
                sta     VTIMR1+1
                plp
                rts

@remove:        php
                sei
                lda     old_VTIMR1_handler
                sta     VTIMR1
                lda     old_VTIMR1_handler+1
                sta     VTIMR1+1
                plp
                rts

.endif  ; .ifdef __ATARIXL__
