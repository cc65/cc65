;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.99

; char DlgBoxGetString       (char *string, char strlen, char *line1,char *line2);

            .export _DlgBoxGetString
            .import DB_get2lines, _DoDlgBox
            .importzp ptr2, ptr3, ptr4
            .import popa, popax

            .include "geossym.inc"
            .include "const.inc"

_DlgBoxGetString:
        jsr DB_get2lines
        jsr popa
        sta DB_strlen
        jsr popax
        sta ptr2
        stx ptr2+1
        lda #<paramStrGetString
        ldx #>paramStrGetString
        jmp _DoDlgBox

.data

paramStrGetString:
        .byte DEF_DB_POS | 1
        .byte DBVARSTR, TXT_LN_X, TXT_LN_1_Y, ptr3
        .byte DBVARSTR, TXT_LN_X, TXT_LN_2_Y, ptr4
        .byte DBGETSTRING, TXT_LN_X, TXT_LN_3_Y, ptr2
DB_strlen:
        .byte 17
        .byte CANCEL, DBI_X_2, DBI_Y_2
        .byte NULL
