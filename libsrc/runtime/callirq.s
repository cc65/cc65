;
; Ullrich von Bassewitz, 2004-04-04
;
; CC65 runtime: Support for calling special irq routines declared as condes
; type 2.
;
; There are two reasons, why this is a separate routine, and the generic
; condes routine in condes.s is not used:
;
;   1. Speed. Having several things hardcoded makes it faster. This is
;      important if it is called in each interrupt.
;
;   2. Reentrancy. The condes routines must use self modyfiying code, which
;      means it is not reentrant. An IRQ using condes, that interrupts
;      another use of condes will cause unpredicatble behaviour. The current
;      code avoids this by using locking mechanisms, but it's complex and
;      has a size and performance penalty.
;
;   3. Special semantics: An interruptor called by callirq must tell by
;      setting or resetting the carry flag if the interrupt has been handled
;      (which means that the interrupt is no longer active at the interrupt
;      source). callirq will call no other interruptors if this happens. To
;      simplify code, all interrupt routines will be called with carry clear
;      on entry.
;
; As the normal condes routine, this one has the limitation of 127 table
; entries.
;

        .export         callirq
        .export         callirq_y       ; Same but with Y preloaded
        .export         __CALLIRQ__ : absolute = 1
        .constructor    irq_init, 10
        .destructor     irq_done, 10

        .import         __INTERRUPTOR_TABLE__, __INTERRUPTOR_COUNT__
        .import         initirq
        .import         doneirq

        irq_init :=     initirq
        irq_done :=     doneirq

; --------------------------------------------------------------------------
; Call all IRQ routines. The function needs to use self modifying code and
; is thereforce placed in the data segment. It will return carry set if the
; interrupt was handled and carry clear if not. The caller may choose to
; ignore this at will.
; NOTE: The routine must not be called if the table is empty!

.data

callirq:
        ldy     #.lobyte(__INTERRUPTOR_COUNT__*2)
callirq_y:
        clc                             ; Preset carry flag
loop:   dey
        lda     __INTERRUPTOR_TABLE__,y
        sta     jmpvec+2                ; Modify code below
        dey
        lda     __INTERRUPTOR_TABLE__,y
        sta     jmpvec+1                ; Modify code below
        sty     index+1                 ; Modify code below
jmpvec: jsr     $FFFF                   ; Patched at runtime
        bcs     done                    ; Bail out if interrupt handled
index:  ldy     #$FF                    ; Patched at runtime
        bne     loop
done:   rts
