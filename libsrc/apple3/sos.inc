;
; Robert Justice, 07.06.2026
;
; Apple SOS Call interface
;

D_READ_CALL     = $80 ;READ_BLOCK
D_READ_COUNT    = 5

D_WRITE_CALL    = $81 ;WRITE_BLOCK
D_WRITE_COUNT   = 4

D_STATUS_CALL   = $82
D_STATUS_COUNT  = 3

D_CONTROL_CALL  = $83
D_CONTROL_COUNT = 3

GET_DEV_NUM_CALL  = $84
GET_DEV_NUM_COUNT = 2

D_INFO_CALL     = $85
D_INFO_COUNT    = 4

GET_TIME_CALL   = $63
GET_TIME_COUNT  = 1

GET_ANALOG_CALL   = $64
GET_ANALOG_COUNT  = 2

CREATE_CALL     = $C0 ;option list
CREATE_COUNT    = 3

DESTROY_CALL    = $C1
DESTROY_COUNT   = 1

RENAME_CALL     = $C2
RENAME_COUNT    = 2

SET_INFO_CALL   = $C3 ;option list
SET_INFO_COUNT  = 3

GET_INFO_CALL   = $C4 ;option list
GET_INFO_COUNT  = 3

VOLUME_CALL     = $C5
VOLUME_COUNT    = 4

SET_PREFIX_CALL = $C6
SET_PREFIX_COUNT = 1

GET_PREFIX_CALL = $C7
GET_PREFIX_COUNT = 2

OPEN_CALL       = $C8 ;option list
OPEN_COUNT      = 4

READ_CALL       = $CA
READ_COUNT      = 4

WRITE_CALL      = $CB
WRITE_COUNT     = 3

CLOSE_CALL      = $CC
CLOSE_COUNT     = 1

SET_MARK_CALL   = $CE
SET_MARK_COUNT  = 3

GET_MARK_CALL   = $CF
GET_MARK_COUNT  = 2

SET_EOF_CALL    = $D0
SET_EOF_COUNT   = 3

GET_EOF_CALL    = $D1
GET_EOF_COUNT   = 2

SET_LEVEL_CALL  = $D2
SET_LEVEL_COUNT = 1

GET_LEVEL_CALL  = $D3
GET_LEVEL_COUNT = 1

        .struct SOS
                .union
                        ; Pseudo-struct to make clear that calls with a
                        ; PATHNAME parameter always have PATHNAME in the
                        ; same place
                        .struct PATH
                                PARAM_COUNT     .byte
                                PATHNAME        .addr
                        .endstruct
                        .struct D_RW
                                PARAM_COUNT     .byte
                                DEV_NUM         .byte
                                DATA_BUFFER     .addr
                                REQUEST_COUNT   .word
                                BLOCK_NUM       .word
                                TRANSFER_COUNT  .word    ; trans count not used for write
                        .endstruct
                        .struct D_STAT_CTRL
                                PARAM_COUNT     .byte
                                DEV_NUM         .byte
                                CODE            .byte
                                LIST            .addr
                        .endstruct
                        .struct GET_DEV_NUM
                                PARAM_COUNT     .byte
                                DEV_NAME        .addr
                                DEV_NUM         .byte
                        .endstruct
                        .struct CREATE
                                PARAM_COUNT     .byte
                                PATHNAME        .addr
                                OPTION_LIST     .addr
                                LENGTH          .byte
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
                        .struct INFO
                                PARAM_COUNT     .byte
                                PATHNAME        .addr
                                OPTION_LIST     .addr
                                LENGTH          .byte
                        .endstruct
                        .struct DINFO
                                PARAM_COUNT     .byte
                                DEV_NUM         .byte
                                DEV_NAME        .addr
                                OPTION_LIST     .addr
                                LENGTH          .byte
                        .endstruct
                        .struct VOLUME
                                PARAM_COUNT     .byte
                                DEV_NAME        .addr
                                VOL_NAME        .addr
                                TOTAL_BLOCKS    .word
                                FREE_BLOCKS     .word
                        .endstruct
                        .struct PREFIX
                                PARAM_COUNT     .byte
                                PATHNAME        .addr
                                LENGTH          .byte
                        .endstruct
                        .struct OPEN
                                PARAM_COUNT     .byte
                                PATHNAME        .addr
                                REF_NUM         .byte
                                OPTION_LIST     .addr
                                LENGTH          .byte
                        .endstruct
                        .struct RW
                                PARAM_COUNT     .byte
                                REF_NUM         .byte
                                DATA_BUFFER     .addr
                                REQUEST_COUNT   .word
                                TRANS_COUNT     .word    ; trans count not used for write
                        .endstruct
                        .struct CLOSE
                                PARAM_COUNT     .byte
                                REF_NUM         .byte
                        .endstruct
                        .struct SET_MARK
                                PARAM_COUNT     .byte
                                REF_NUM         .byte
                                BASE            .byte
                                POSITION        .byte 4
                        .endstruct
                        .struct GET_MARK
                                PARAM_COUNT     .byte
                                REF_NUM         .byte
                                POSITION        .byte 4
                        .endstruct
                        .struct SET_EOF
                                PARAM_COUNT     .byte
                                REF_NUM         .byte
                                BASE            .byte
                                EOF             .byte 4
                        .endstruct
                        .struct GET_EOF
                                PARAM_COUNT     .byte
                                REF_NUM         .byte
                                EOF             .byte 4
                        .endstruct
                        .struct LEVEL
                                PARAM_COUNT     .byte
                                LEVEL           .byte
                        .endstruct
                        .struct GET_ANALOG
                                PARAM_COUNT     .byte
                                JOY_MODE        .byte
                                JOY_STATUS      .byte 4
                        .endstruct
                .endunion
        .endstruct

        .struct OPTION
                .union
                        .struct CREATE
                                FILE_TYPE       .byte
                                AUX_TYPE        .word
                                STORAGE_TYPE    .byte
                                EOF             .byte 4
                        .endstruct
                        .struct FILE_INFO
                                ACCESS          .byte
                                FILE_TYPE       .byte
                                AUX_TYPE        .word
                                STORAGE_TYPE    .byte
                                EOF             .byte 4
                                BLOCKS_USED     .word
                                LAST_MOD        .byte 4
                        .endstruct
                        .struct OPEN
                                REQ_ACCESS      .byte
                                PAGES           .byte
                                IO_BUFFER       .addr 2  ;;check doc is unclear
                        .endstruct
                        .struct DINFO
                                SLOT_NUM        .byte
                                UNIT_NUM        .byte
                                DEV_TYPE        .byte
                                SUB_TYPE        .byte
                                UNUSED          .byte
                                EOF             .byte 4
                                TOTAL_BLOCKS    .word
                                MANUF_ID        .word
                                VERSION_NUM     .word
                        .endstruct
                .endunion
        .endstruct

        .global         sosparam
        .global         sosoption
        .global         callsos

        ; Assert that PATHNAME parameters is always at the same place
        .assert SOS::CREATE::PATHNAME  = SOS::PATH::PATHNAME, error
        .assert SOS::DESTROY::PATHNAME = SOS::PATH::PATHNAME, error
        .assert SOS::INFO::PATHNAME    = SOS::PATH::PATHNAME, error
        .assert SOS::PREFIX::PATHNAME  = SOS::PATH::PATHNAME, error
        .assert SOS::OPEN::PATHNAME    = SOS::PATH::PATHNAME, error

; Max filename length
FILENAME_MAX =  64+1
