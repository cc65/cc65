;
; Startup code for cc65 (supervision version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

	.import	        _main
        .import         initlib, donelib, copydata
        .import         zerobss
	.import		__RAM_START__, __RAM_SIZE__	; Linker generated

	.include "zeropage.inc"
	.include "supervision.inc"

	.export	_sv_irq_timer_counter, _sv_irq_dma_counter
	.export	_sv_nmi_counter

.bss

_sv_irq_dma_counter:    .byte 0
_sv_irq_timer_counter:  .byte 0
_sv_nmi_counter:        .byte 0

.code

reset:
	jsr	zerobss

	; initialize data
	jsr	copydata

	lda	#>(__RAM_START__ + __RAM_SIZE__)
       	sta	sp+1   		; Set argument stack ptr
       	stz	sp              ; #<(__RAM_START__ + __RAM_SIZE__)
	jsr	initlib
	jsr	_main
_exit:	jsr     donelib
exit:   jmp    	exit


.proc   irq
	pha
	lda	sv_irq_source
	and	#SV_IRQ_REQUEST_TIMER
	beq	not_timer
	lda	sv_timer_quit
	inc	_sv_irq_timer_counter
not_timer:
	lda	sv_irq_source
	and	#SV_IRQ_REQUEST_DMA
	beq	not_dma
	lda	sv_dma_quit
	inc	_sv_irq_dma_counter
not_dma:
	pla
	rti
.endproc

.proc   nmi
	inc	_sv_nmi_counter
	rti
.endproc

; removing this segment gives only a warning
        .segment "FFF0"
.proc reset32kcode
        lda     #(6<<5)
        sta     sv_bank
; now the 32kbyte image can reside in the top of 64kbyte, 128kbyte roms
        jmp     reset
.endproc

        .segment "VECTOR"

.word   nmi
.word   reset32kcode
.word   irq


