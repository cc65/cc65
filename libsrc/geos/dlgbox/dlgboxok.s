
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char DlgBoxOk       (char *line1,char *line2);

	    .export _DlgBoxOk
	    .import DB_get2lines, _DoDlgBox
	    .importzp ptr3, ptr4

	    .include "../inc/geossym.inc"
	    .include "../inc/const.inc"

_DlgBoxOk:
	    jsr DB_get2lines
	    lda #<paramStrOk
	    ldx #>paramStrOk
	    jmp _DoDlgBox

paramStrOk:
	    .byte DEF_DB_POS | 1
	    .byte DBVARSTR, TXT_LN_X, TXT_LN_2_Y, ptr3
	    .byte DBVARSTR, TXT_LN_X, TXT_LN_3_Y, ptr4
	    .byte OK, DBI_X_0, DBI_Y_2
	    .byte NULL
