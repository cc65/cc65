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

.segment        "INIT"

initirq:
        lda     VVBLKI
        ldx     VVBLKI+1
        sta     IRQInd+1
        stx     IRQInd+2
        lda     #6
        ldy     #<IRQStub
        ldx     #>IRQStub
        jsr     SETVBV
        rts

; ------------------------------------------------------------------------

.code

doneirq:
        lda     #6
        ldy     IRQInd+1
        ldx     IRQInd+2
        jsr     SETVBV
        rts

; ------------------------------------------------------------------------

.segment        "LOWCODE"

IRQStub:
        cld                             ; Just to be sure
.ifdef __ATARIXL__
        pha
.ifdef CHARGEN_RELOC
        lda     CHBAS
        pha
.endif
        lda     PORTB
        pha
        and     #$FE
        sta     PORTB                   ; disable ROM
        set_chbase >__CHARGEN_START__
.endif
        jsr     callirq                 ; Call the functions
.ifdef __ATARIXL__
        pla
        sta     PORTB                   ; restore old ROM setting
.ifdef CHARGEN_RELOC
        pla
        sta     CHBAS
        sta     CHBASE
.endif
        pla
.endif
        jmp     IRQInd                  ; Jump to the saved IRQ vector

; ------------------------------------------------------------------------

.data

IRQInd: jmp     $0000
