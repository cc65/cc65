
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char DlgBoxFileSelect       (char *class, char ftype, char *fname);

		.export _DlgBoxFileSelect
		.import popa, popax

		.include "../inc/jumptab.inc"
		.include "../inc/geossym.inc"
		.include "../inc/const.inc"
		.include "../inc/geosmac.ca65.inc"

_DlgBoxFileSelect:
;	    sta r5L
;	    stx r5H
;	    jsr popa
;	    sta r7L
;	    jsr popax
;	    sta r10L
;	    stx r10H

	        sta tmp_r5
	        stx tmp_r5+1
	        jsr popa
	        sta tmp_r7L
	        jsr popax
	        sta tmp_r10
	        stx tmp_r10+1

DB_FS_reload:
	        MoveW tmp_r5, r5
	        MoveW tmp_r10, r10
	        MoveB tmp_r7L, r7L

		lda #<paramStrFileSelect
		ldx #>paramStrFileSelect
		sta r0L
		stx r0H
		jsr DoDlgBox
		lda r0L
		cmp #DISK
		bne DB_FS_Fin
		jsr OpenDisk
		txa
		beq DB_FS_reload
DB_FS_Fin:  	rts

paramStrFileSelect:
		.byte DEF_DB_POS | 1
		.byte DBGETFILES, 4, 4
		.byte OPEN, DBI_X_2, DBI_Y_0+16
		.byte DISK, DBI_X_2, DBI_Y_0+32+1
		.byte CANCEL, DBI_X_2, DBI_Y_0+64+3
		.byte NULL

tmp_r5:		.word 0
tmp_r7L:	.byte 0
tmp_r10:	.word 0
