;
; Ullrich von Bassewitz, 20.11.2000
;
; CC65 runtime: Support for calling module constructors/destructors
;
; The condes routine must be called with the table address in a/x and the
; size of the table in y. The current implementation limits the table size
; to 254 bytes (127 vectors) but this shouldn't be problem for now and may
; be changed later.
;
; libinit and libdone call condes with the predefined module constructor and
; destructor tables, they must be called from the platform specific startup
; code.
;
; The function does also export jmpvec as general purpose jump vector that
; lies in the data segment so it's address may be patched at runtime.
;

       	.export	initlib, donelib, condes
       	.export	jmpvec

       	.import	__CONSTRUCTOR_TABLE__, __CONSTRUCTOR_COUNT__
	.import	__DESTRUCTOR_TABLE__, __DESTRUCTOR_COUNT__



.code

; --------------------------------------------------------------------------
; Initialize library modules

.proc	initlib

	lda    	#<__CONSTRUCTOR_TABLE__
	ldx    	#>__CONSTRUCTOR_TABLE__
	ldy    	#<(__CONSTRUCTOR_COUNT__*2)
	bne    	condes
	rts

.endproc


; --------------------------------------------------------------------------
; Cleanup library modules

.proc	donelib

	lda 	#<__DESTRUCTOR_TABLE__
	ldx 	#>__DESTRUCTOR_TABLE__
	ldy 	#<(__DESTRUCTOR_COUNT__*2)
	bne 	condes
	rts

.endproc


; --------------------------------------------------------------------------
; Generic table call handler

.proc	condes

	sta   	getbyt+1
	stx	getbyt+2
	sty	index

loop:	ldy	index
     	beq	done
	dey
	jsr	getbyt
	sta	jmpvec+2
	dey
	jsr	getbyt
	sta	jmpvec+1
	sty	index
	jsr	jmpvec
	jmp	loop

done:	rts

.endproc


; --------------------------------------------------------------------------
; Data. The getbyte routine is placed in the data segment cause it's patched
; at runtime.

.bss

index:	.byte	0

.data

getbyt:	lda	$FFFF,y
	rts

jmpvec:	jmp	$FFFF


