
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char DlgBoxOkCancel       (char *line1,char *line2);

	    .export _DlgBoxOkCancel
	    .import DB_get2lines
	    .importzp ptr3, ptr4

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	    .include "../inc/const.inc"

_DlgBoxOkCancel:
	    jsr DB_get2lines
	    lda #<paramStrOkCancel
	    ldx #>paramStrOkCancel
	    sta r0L
	    stx r0H
	    jsr DoDlgBox
	    lda r0L
	    rts

paramStrOkCancel:
	    .byte DEF_DB_POS | 1
	    .byte DBVARSTR, TXT_LN_X, TXT_LN_2_Y, ptr3
	    .byte DBVARSTR, TXT_LN_X, TXT_LN_3_Y, ptr4
	    .byte OK, DBI_X_0, DBI_Y_2
	    .byte CANCEL, DBI_X_2, DBI_Y_2
	    .byte NULL
