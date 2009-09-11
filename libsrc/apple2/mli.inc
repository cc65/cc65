;
; Oliver Schmidt, 30.12.2004
;
; Apple ProDOS 8 MLI
;

READ_BLOCK_CALL = $80
WRITE_BLOCK_CALL= $81
RW_BLOCK_COUNT  = 3

GET_TIME_CALL   = $82
GET_TIME_COUNT  = 0

CREATE_CALL     = $C0
CREATE_COUNT    = 7

DESTROY_CALL    = $C1
DESTROY_COUNT   = 1

RENAME_CALL     = $C2
RENAME_COUNT    = 2

ON_LINE_CALL    = $C5
ON_LINE_COUNT   = 2

SET_PREFIX_CALL = $C6
GET_PREFIX_CALL = $C7
PREFIX_COUNT    = 1

OPEN_CALL       = $C8
OPEN_COUNT      = 3

READ_CALL       = $CA
WRITE_CALL      = $CB
RW_COUNT        = 4

CLOSE_CALL      = $CC
CLOSE_COUNT     = 1

SET_MARK_CALL   = $CE
GET_MARK_CALL   = $CF
MARK_COUNT      = 2

SET_EOF_CALL    = $D0
GET_EOF_CALL    = $D1
EOF_COUNT       = 2

        .struct MLI
                .union
                        .struct RW_BLOCK
                                PARAM_COUNT     .byte
                                UNIT_NUM        .byte
                                DATA_BUFFER     .addr
                                BLOCK_NUM       .word
                        .endstruct
                        .struct CREATE
                                PARAM_COUNT     .byte
                                PATHNAME        .addr
                                ACCESS          .byte
                                FILE_TYPE       .byte
                                AUX_TYPE        .word
                                STORAGE_TYPE    .byte
                                CREATE_DATE     .word
                                CREATE_TIME     .word
                        .endstruct
                        .struct DESTROY
                                PARAM_COUNT     .byte
                                PATHNAME        .addr
                        .endstruct
                        .struct RENAME
                                PARAM_COUNT     .byte
                                PATHNAME        .addr
                                NEW_PATHNAME    .addr
                        .endstruct
                        .struct ON_LINE
                                PARAM_COUNT     .byte
                                UNIT_NUM        .byte
                                DATA_BUFFER     .addr
                        .endstruct
                        .struct PREFIX
                                PARAM_COUNT	.byte
                                PATHNAME	.addr
                        .endstruct
                        .struct OPEN
                                PARAM_COUNT     .byte
                                PATHNAME        .addr
                                IO_BUFFER       .addr
                                REF_NUM         .byte
                        .endstruct
                        .struct RW
                                PARAM_COUNT     .byte
                                REF_NUM         .byte
                                DATA_BUFFER     .addr
                                REQUEST_COUNT   .word
                                TRANS_COUNT     .word
                        .endstruct
                        .struct CLOSE
                                PARAM_COUNT     .byte
                                REF_NUM         .byte
                        .endstruct
                        .struct MARK
                                PARAM_COUNT     .byte
                                REF_NUM         .byte
                                POSITION        .byte 3
                        .endstruct
                        .struct EOF
                                PARAM_COUNT     .byte
                                REF_NUM         .byte
                                EOF             .byte 3
                        .endstruct
                .endunion
        .endstruct

        .global         mliparam
        .global         callmli

DEVNUM  :=      $BF30		; Most recent accessed device
DATELO  :=      $BF90		; Bits 15-9 = Year, 8-5 = Month, 4-0 = Day
TIMELO  :=      $BF92		; Bits 12-8 = Hour, 5-0 = Minute
PFIXPTR :=      $BF9A		; If = 0, no prefix active
KVERSION:=      $BFFF           ; Kernel version number
