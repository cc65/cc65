;
; Oliver Schmidt, 30.12.2004
;
; Apple ProDOS 8 MLI
;

CREATE_CALL     = $C0
CREATE_COUNT    = 7

ON_LINE_CALL    = $C5
ON_LINE_COUNT   = 2

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
                        .struct CREATE
                                PARAM_COUNT	.byte
                                PATHNAME	.addr
                                ACCESS		.byte
                                FILE_TYPE	.byte
                                AUX_TYPE	.word
                                STORAGE_TYPE	.byte
                                CREATE_DATE	.word
                                CREATE_TIME	.word
                        .endstruct
                        .struct	ON_LINE
                                PARAM_COUNT	.byte
                                UNIT_NUM	.byte
                                DATA_BUFFER	.addr
                        .endstruct
                        .struct OPEN
                                PARAM_COUNT	.byte
                                PATHNAME	.addr
                                IO_BUFFER	.addr
                                REF_NUM		.byte
                        .endstruct
                        .struct RW
                                PARAM_COUNT	.byte
                                REF_NUM		.byte
                                DATA_BUFFER	.addr
                                REQUEST_COUNT	.word
                                TRANS_COUNT	.word
                        .endstruct
                        .struct CLOSE
                                PARAM_COUNT	.byte
                                REF_NUM		.byte
                        .endstruct
                        .struct MARK
                                PARAM_COUNT	.byte
                                REF_NUM		.byte
                                POSITION	.byte 3
                        .endstruct
                        .struct EOF
                                PARAM_COUNT	.byte
                                REF_NUM		.byte
                                EOF		.byte 3
                        .endstruct
                .endunion
        .endstruct

        .global		mliparam
        .global		callmli

ENTRY   :=	$BF00		; MLI call entry point
DEVNUM  :=	$BF30		; Most recent accessed device
PFIXPTR :=	$BF9A		; If = 0, no prefix active
KVERSION:=      $BFFF		; Kernel version number
