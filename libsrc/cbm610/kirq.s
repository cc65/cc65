;
; Ullrich von Bassewitz, 28.09.1998
;
; IRQ routine for the 610.
;

 	.export	  	irq, nmi, k_irq, k_nmi
	.import		k_scnkey, k_udtim, k_rs232
	.importzp     	tpi1

	.include      	"zeropage.inc"
	.include	"io.inc"
	.include	"page3.inc"


; -------------------------------------------------------------------------
; This is the mapping of the active irq register of the	6525 (tpi1):
;
; Bit   7       6       5       4       3       2       1       0
;                               |       |       |       |       ^ 50 Hz
;                               |       |       |       ^ SRQ IEEE 488
;                               |       |       ^ cia
;                               |       ^ IRQB ext. Port
;                               ^ acia



; -------------------------------------------------------------------------
; IRQ entry point

.proc	irq

	pha
        txa
        pha
        tya
        pha
	tsx
	lda	$104,x	    		; Get the flags from the stack
	and	#$10	    		; Test break flag
	bne	L1
	jmp	(IRQVec)
L1:	jmp	(BRKVec)

.endproc

; -------------------------------------------------------------------------
; NMI entry point

.proc	nmi

	jmp	(NMIVec)

.endproc


; -------------------------------------------------------------------------
; Kernal irq entry point. The IRQvec points here (usually).

k_irq:
	lda     IndReg                  ; Ind. Segment retten
        pha
        cld
	lda	#$0F
	sta	IndReg
	ldy	#tpiActIntReg
	lda	(tpi1),y		; Interrupt Register 6525
	beq	noirq

; -------------------------------------------------------------------------
; 50/60Hz interrupt

	cmp	#%00000001		; ticker irq?
	bne	irq1
	jsr     k_scnkey		; Poll the keyboard
        jsr	k_udtim			; Bump the time

; -------------------------------------------------------------------------
; UART interrupt

irq1:	cmp	#%00010000		; interrupt from uart?
     	bne	irqend
	jsr	k_rs232			; Read character from uart

; -------------------------------------------------------------------------
; Done

irqend:	ldy	#tpiActIntReg
       	sta	(tpi1),y		; Clear interrupt

noirq:	pla
        sta     IndReg
        pla
        tay
        pla
        tax
        pla
k_nmi:	rti


