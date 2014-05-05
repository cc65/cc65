;
; IRQ handling (C128 version)
;

        .export         initirq, doneirq
        .import         callirq

        .include        "c128.inc"

; ------------------------------------------------------------------------

.segment        "INIT"

initirq:
        lda     IRQVec
        ldx     IRQVec+1
        sta     IRQInd+1
        stx     IRQInd+2
        lda     #<IRQStub
        ldx     #>IRQStub
        jmp     setvec

; ------------------------------------------------------------------------

.code

doneirq:
        lda     IRQInd+1
        ldx     IRQInd+2
setvec: sei
        sta     IRQVec
        stx     IRQVec+1
        cli
        rts

; ------------------------------------------------------------------------
; The C128 has ROM parallel to the RAM starting from $4000. The startup code
; above will change this setting so that we have RAM from $0000-$BFFF. This
; works quite well with the exception of interrupts: The interrupt handler
; is in ROM, and the ROM switches back to the ROM configuration, which means
; that parts of our program may not be accessible. To solve this, we place
; the following code into a special segment called "LOWCODE" which will be
; placed just above the startup code, so it goes into a RAM area that is
; not banked.

.segment        "LOWCODE"

IRQStub:
        cld                             ; Just to be sure
        lda     MMU_CR                  ; Get old register value
        pha                             ; And save on stack
        lda     #MMU_CFG_CC65           ; Bank 0 with kernal ROM
        sta     MMU_CR
        jsr     callirq                 ; Call the functions
        pla                             ; Get old register value
        sta     MMU_CR
        jmp     IRQInd                  ; Jump to the saved IRQ vector
