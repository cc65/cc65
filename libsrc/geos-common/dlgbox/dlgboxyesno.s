;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.99

; char DlgBoxYesNo       (char *line1,char *line2);

            .export _DlgBoxYesNo
            .import DB_get2lines, _DoDlgBox
            .importzp ptr3, ptr4

            .include "geossym.inc"
            .include "const.inc"

_DlgBoxYesNo:
        jsr DB_get2lines
        lda #<paramStrYesNo
        ldx #>paramStrYesNo
        jmp _DoDlgBox

.rodata

paramStrYesNo:
        .byte DEF_DB_POS | 1
        .byte DBVARSTR, TXT_LN_X, TXT_LN_2_Y, ptr3
        .byte DBVARSTR, TXT_LN_X, TXT_LN_3_Y, ptr4
        .byte YES, DBI_X_0, DBI_Y_2
        .byte NO, DBI_X_2, DBI_Y_2
        .byte NULL
