;
; Maciej 'YTM/Elysium' Witkowiak <ytm@elysium.pl>
; 23.12.2002
;
; Zero the bss segment.
;

	.export		zerobss
	.import		__BSS_RUN__, __BSS_SIZE__

        .include "../inc/jumptab.inc"
        .include "../inc/geossym.inc"

.code

zerobss:
	    lda	#<__BSS_SIZE__
	    ldx #>__BSS_SIZE__
	    sta r0L
	    stx r0H
	    lda #0
	    sta r2L
	    lda #<__BSS_RUN__
	    ldx #>__BSS_RUN__
	    sta r1L
	    stx r1H
	    jmp FillRam
