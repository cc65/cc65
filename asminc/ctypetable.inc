; ctypetable.inc
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; Data covering all possible combinations of character flags for target specific definition
;

.include    "ctype.inc"
.export     __ctype

; Table definition covering all possible ctype combinations

.rodata
__ctype:
ct_none:                .byte CT_NONE
ct_lower:               .byte CT_LOWER
ct_upper:               .byte CT_UPPER
ct_digit_xdigit:        .byte CT_DIGIT | CT_XDIGIT
ct_lower_xdigit:        .byte CT_LOWER | CT_XDIGIT
ct_upper_xdigit:        .byte CT_UPPER | CT_XDIGIT
ct_ctrl:                .byte CT_CTRL
ct_ws:                  .byte CT_OTHER_WS
ct_ctrl_ws:             .byte CT_CTRL | CT_OTHER_WS
ct_space_spacetab:      .byte CT_SPACE | CT_SPACE_TAB
ct_ctrl_ws_spacetab:    .byte CT_CTRL | CT_OTHER_WS | CT_SPACE_TAB

; build indices out of the table above:

CT_NONE_IDX             = ct_none - __ctype
CT_LOWER_IDX            = ct_lower - __ctype
CT_UPPER_IDX            = ct_upper - __ctype
CT_DIGIT_XDIGIT_IDX     = ct_digit_xdigit - __ctype
CT_LOWER_XDIGIT_IDX     = ct_lower_xdigit - __ctype
CT_UPPER_XDIGIT_IDX     = ct_upper_xdigit - __ctype
CT_CTRL_IDX             = ct_ctrl - __ctype
CT_WS_IDX               = ct_ws - __ctype
CT_CTRL_WS_IDX          = ct_ctrl_ws - __ctype
CT_SPACE_SPACETAB_IDX   = ct_space_spacetab - __ctype
CT_CTRL_WS_SPACETAB_IDX = ct_ctrl_ws_spacetab - __ctype

.macro ct_mix lower, upper
        .byte ((lower) & $0F) | ((upper) << 4)
.endmacro
