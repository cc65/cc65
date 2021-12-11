;
; IRQ handling (ATARI version)
;

        .export         initirq, doneirq
        .import         callirq

        .include        "atari.inc"
.ifdef __ATARIXL__
        .import         __CHARGEN_START__
        .include        "romswitch.inc"
.endif

; ------------------------------------------------------------------------

.segment        "ONCE"

initirq:
        lda     #$4C                    ; JMP opcode
        sta     IRQInd
        lda     VVBLKD
        ldx     VVBLKD+1
        sta     IRQInd+1
        stx     IRQInd+2
        lda     #7
        ldy     #<IRQStub
        ldx     #>IRQStub
        jmp     SETVBV

; ------------------------------------------------------------------------

.code

doneirq:
        lda     #7
        ldy     IRQInd+1
        ldx     IRQInd+2
        jmp     SETVBV

; ------------------------------------------------------------------------

.segment        "LOWCODE"

IRQStub:
        cld                             ; Just to be sure
.ifdef __ATARIXL__
.ifdef CHARGEN_RELOC
        lda     CHBAS
        pha
.endif
.endif
        lda     PORTB
        pha
.ifdef __ATARIXL__
        and     #$FE                    ; disable ROM
.endif
        ora     #$10                    ; map main memory into $4000..$7FFF area
        sta     PORTB
.ifdef __ATARIXL__
        set_chbase >__CHARGEN_START__
.endif
        jsr     callirq                 ; Call the functions
        pla
        sta     PORTB                   ; restore old memory settings
.ifdef __ATARIXL__
.ifdef CHARGEN_RELOC
        pla
        sta     CHBAS
        sta     CHBASE
.endif
.endif
        jmp     IRQInd                  ; Jump to the saved IRQ vector

; ------------------------------------------------------------------------

.segment        "LOWBSS"

IRQInd: .res    3

.end
