
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char DlgBoxYesNo       (char *line1,char *line2);

	    .export _DlgBoxYesNo
	    .import DB_get2lines
	    .importzp ptr3, ptr4

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	    .include "../inc/const.inc"

_DlgBoxYesNo:
	    jsr DB_get2lines
	    lda #<paramStrYesNo
	    ldx #>paramStrYesNo
	    sta r0L
	    stx r0H
	    jsr DoDlgBox
	    lda r0L
	    rts

paramStrYesNo:
	    .byte DEF_DB_POS | 1
	    .byte DBVARSTR, TXT_LN_X, TXT_LN_2_Y, ptr3
	    .byte DBVARSTR, TXT_LN_X, TXT_LN_3_Y, ptr4
	    .byte YES, DBI_X_0, DBI_Y_2
	    .byte NO, DBI_X_2, DBI_Y_2
	    .byte NULL
