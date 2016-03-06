;
; IRQ handling (ATARI 5200 version)
;

        .export         initirq, doneirq
        .import         callirq

        .include        "atari5200.inc"

; ------------------------------------------------------------------------

.segment        "ONCE"

initirq:
        lda     VVBLKD
        ldx     VVBLKD+1
        sta     IRQInd+1
        stx     IRQInd+2
        ldy     #<IRQStub
        ldx     #>IRQStub
        jmp     SETVBV

; ------------------------------------------------------------------------

.code

doneirq:
        ldy     IRQInd+1
        ldx     IRQInd+2
        ;jmp     SETVBV
        ; fall thru

; ------------------------------------------------------------------------
; Set deferred vertical blank interrupt
; logic copied from Atari computer ROM

SETVBV: txa
        ldx     #5
        sta     WSYNC                   ; waste 20 CPU cycles
@1:     dex                             ; to allow VBLANK to happen
        bne     @1                      ; if this is line "7C"
        sta     VVBLKD+1
        sty     VVBLKD
        rts

; ------------------------------------------------------------------------

.segment        "LOWCODE"

IRQStub:
        cld                             ; Just to be sure
        jsr     callirq                 ; Call the functions
        jmp     IRQInd                  ; Jump to the saved IRQ vector

; ------------------------------------------------------------------------

.data

IRQInd: jmp     $0000
