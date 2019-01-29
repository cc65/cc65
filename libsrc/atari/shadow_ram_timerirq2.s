;
; Atari XL shadow RAM timer IRQ #2 handler
;
; Christian Groessler, chris@groessler.org, 2019
;

;DEBUG           =       1

.ifdef __ATARIXL__

SHRAM_HANDLERS  =       1
                .include        "atari.inc"
                .include        "romswitch.inc"
                .export         set_VTIMR2_handler


.segment "LOWBSS"

VTIMR2_handler: .res    3


.segment "BSS"

old_VTIMR2_handler:
                .res    2


.segment "LOWCODE"

; timer interrupt handler:
; disable ROM, call user handler, enable ROM again

my_VTIMR2_handler:
                disable_rom_quick
                jsr     VTIMR2_handler
                enable_rom_quick
                pla
                rti

.segment "CODE"

; install or remove VTIMR2 handler
; input: CF - 0/1 for remove/install handler
;        AX - pointer to handler (if CF=1)
; registers destroyed

set_VTIMR2_handler:

                bcc     @remove

; install vector

                stx     VTIMR2_handler+2
                sta     VTIMR2_handler+1        ; save passed vector in low memory
                lda     #$4C                    ; "JMP" opcode
                sta     VTIMR2_handler

                lda     VTIMR2
                sta     old_VTIMR2_handler
                lda     VTIMR2+1
                sta     old_VTIMR2_handler+1

                lda     #<my_VTIMR2_handler
                php
                sei
                sta     VTIMR2
                lda     #>my_VTIMR2_handler
                sta     VTIMR2+1
                plp
                rts

@remove:        php
                sei
                lda     old_VTIMR2_handler
                sta     VTIMR2
                lda     old_VTIMR2_handler+1
                sta     VTIMR2+1
                plp
                rts

.endif  ; .ifdef __ATARIXL__
