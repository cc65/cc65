;
; Maciej 'YTM/Elysium' Witkowiak
; 2.7.2001
;
; this function is used by dio_read and dio_write to fix parameters (secnum)
;

	.import		popax,pushax,_dio_log_to_phys
 	.export		dio_params,dio_secnum
	.include "../inc/geossym.inc"

.bss
dio_secnum:	.res 2

.code
.proc	dio_params

	sta r4L
	stx r4H

	jsr popax
	sta dio_secnum
	stx dio_secnum
	lda #<dio_secnum
	ldx #>dio_secnum
	jsr pushax

	lda #<r1L
	ldx #>r1H
	jmp _dio_log_to_phys

.endproc
