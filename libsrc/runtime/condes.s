;
; Ullrich von Bassewitz, 20.11.2000
;
; CC65 runtime: Support for calling module constructors/destructors
;
; The condes routine must be called with the table address in a/x and the
; number of vectors in the table in y. The current implementation limits
; the table size to 128 bytes (64 vectors) but this shouldn't be problem
; for now and may be changed later.
;

       	.export	condes
       	.import	jmpvec

.code

condes:	sta	getbyt+1
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
	jsr	jmpvec
	jmp	loop

done:	rts

; --------------------------------------------------------------------------
; Data. The getbyte routine is placed in the data segment cause it's patched
; at runtime.

.bss

index:	.byte	0

.data

getbyt:	lda	$FFFF,y
	rts

