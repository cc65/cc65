;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.99

; char DlgBoxOkCancel       (char *line1,char *line2);

            .export _DlgBoxOkCancel
            .import DB_get2lines, _DoDlgBox
            .importzp ptr3, ptr4

            .include "geossym.inc"
            .include "const.inc"

_DlgBoxOkCancel:
        jsr DB_get2lines
        lda #<paramStrOkCancel
        ldx #>paramStrOkCancel
        jmp _DoDlgBox

.rodata

paramStrOkCancel:
        .byte DEF_DB_POS | 1
        .byte DBVARSTR, TXT_LN_X, TXT_LN_2_Y, ptr3
        .byte DBVARSTR, TXT_LN_X, TXT_LN_3_Y, ptr4
        .byte OK, DBI_X_0, DBI_Y_2
        .byte CANCEL, DBI_X_2, DBI_Y_2
        .byte NULL
