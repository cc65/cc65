;
; Maciej 'YTM/Elysium' Witkowiak
; 2.7.2001
;
; this function is used by dio_read and dio_write to fix parameters (secnum)
; this function calls SetDevice so that more than one drive can be used at once

	.import		popax,pushax,_dio_log_to_phys
	.importzp	ptr1
 	.export		dio_params,dio_secnum
	.include 	"../inc/geossym.inc"
	.include	"../inc/jumptab.inc"
	.include	"../inc/dio.inc"

.bss
dio_secnum:	.res 2

.code
.proc	dio_params

	sta r4L
	stx r4H

	jsr popax
	sta dio_secnum
	stx dio_secnum

	jsr popax		; get 3rd parameter
	jsr pushax		; put it back
	sta ptr1
	stx ptr1+1
	ldy #sst_driveno
	lda (ptr1),y
	clc
	adc #8
	jsr SetDevice		; setup device, load driver

	lda #<dio_secnum
	ldx #>dio_secnum
	jsr pushax

	lda #<r1L
	ldx #>r1H
	jmp _dio_log_to_phys

.endproc
