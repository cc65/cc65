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
; As the normal condes routine, this one has the limitation of 127 table
; entries.
;

       	.export	callirq
        .export callirq_y       ; Same but with Y preloaded

       	.import	__IRQFUNC_TABLE__, __IRQFUNC_COUNT__

.code

; --------------------------------------------------------------------------
; Call all IRQ routines. The function needs to use self modifying code and
; is thereforce placed in the data segment.
; NOTE: The routine must not be called if the table is empty!

.data

callirq:
        ldy     #.lobyte(__IRQFUNC_COUNT__*2)
callirq_y:
        dey
        lda     __IRQFUNC_TABLE__,y
        sta     jmpvec+2                ; Modify code below
     	dey
        lda     __IRQFUNC_TABLE__,y
        sta     jmpvec+1                ; Modify code below
       	sty    	index+1                 ; Modify code below
jmpvec: jsr    	$FFFF                   ; Patched at runtime
index: 	ldy    	#$FF                    ; Patched at runtime
       	bne     callirq_y
        rts



