;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                               ;
; LOADER.SYSTEM - an Apple][ ProDOS 8 loader for cc65 programs (Oliver Schmidt) ;
;                                                                               ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

A1L             := $3C
A1H             := $3D
STACK           := $0100
BUF             := $0200
PATHNAME        := $0280
MLI             := $BF00
VERSION         := $FBB3
RDKEY           := $FD0C
PRBYTE          := $FDDA
COUT            := $FDED

QUIT_CALL          = $65
GET_FILE_INFO_CALL = $C4
OPEN_CALL          = $C8
READ_CALL          = $CA
CLOSE_CALL         = $CC
FILE_NOT_FOUND_ERR = $46

; ------------------------------------------------------------------------

        .import __CODE_0300_SIZE__, __DATA_0300_SIZE__
        .import __CODE_0300_LOAD__, __CODE_0300_RUN__

; ------------------------------------------------------------------------

        .segment        "DATA_2000"

GET_FILE_INFO_PARAM:
                .byte   $0A             ;PARAM_COUNT
                .addr   PATHNAME        ;PATHNAME
                .byte   $00             ;ACCESS
                .byte   $00             ;FILE_TYPE
FILE_INFO_ADDR: .word   $0000           ;AUX_TYPE
                .byte   $00             ;STORAGE_TYPE
                .word   $0000           ;BLOCKS_USED
                .word   $0000           ;MOD_DATE
                .word   $0000           ;MOD_TIME
                .word   $0000           ;CREATE_DATE
                .word   $0000           ;CREATE_TIME

OPEN_PARAM:
                .byte   $03             ;PARAM_COUNT
                .addr   PATHNAME        ;PATHNAME
                .addr   MLI - 1024      ;IO_BUFFER
OPEN_REF:       .byte   $00             ;REF_NUM

LOADING:
                .byte   $0D
                .asciiz "Loading "

ELLIPSES:
                .byte   " ...", $0D, $0D, $00

; ------------------------------------------------------------------------

        .segment        "DATA_0300"

READ_PARAM:
                .byte   $04             ;PARAM_COUNT
READ_REF:       .byte   $00             ;REF_NUM
READ_ADDR:      .addr   $0000           ;DATA_BUFFER
                .word   $FFFF           ;REQUEST_COUNT
                .word   $0000           ;TRANS_COUNT

CLOSE_PARAM:
                .byte   $01             ;PARAM_COUNT
CLOSE_REF:      .byte   $00             ;REF_NUM

QUIT_PARAM:
                .byte   $04             ;PARAM_COUNT
                .byte   $00             ;QUIT_TYPE
                .word   $0000           ;RESERVED
                .byte   $00             ;RESERVED
                .word   $0000           ;RESERVED

FILE_NOT_FOUND:
                .asciiz "... File Not Found"

ERROR_NUMBER:
                .asciiz "... Error $"

PRESS_ANY_KEY:
                .asciiz " - Press Any Key "

; ------------------------------------------------------------------------

        .segment        "CODE_2000"

        jmp     :+
        .byte   $EE
        .byte   $EE
        .byte   $7F
STARTUP:.res    $7F

        ; Reset stack
:       ldx     #$FF
        txs

        ; Relocate CODE_0300 and DATA_0300
        ldx     #<(__CODE_0300_SIZE__ + __DATA_0300_SIZE__)
:       lda     __CODE_0300_LOAD__-1,x
        sta     __CODE_0300_RUN__-1,x
        dex
        bne     :-

        ; Remove ".SYSTEM" from pathname
        lda     PATHNAME
        sec
        sbc     #.strlen(".SYSTEM")
        sta     PATHNAME

        ; Add trailing '\0' to pathname
        tax
        lda     #$00
        sta     PATHNAME+1,x

        ; Copy ProDOS startup filename and trailing '\0' to stack
        ldx     STARTUP
        lda     #$00
        beq     :++             ; Branch always
:       lda     STARTUP+1,x
:       sta     STACK,x
        dex
        bpl     :--

        ; Provide some user feedback
        lda     #<LOADING
        ldx     #>LOADING
        jsr     PRINT
        lda     #<(PATHNAME+1)
        ldx     #>(PATHNAME+1)
        jsr     PRINT
        lda     #<ELLIPSES
        ldx     #>ELLIPSES
        jsr     PRINT

        jsr     MLI
        .byte   GET_FILE_INFO_CALL
        .word   GET_FILE_INFO_PARAM
        bcc     :+
        jmp     ERROR

:       jsr     MLI
        .byte   OPEN_CALL
        .word   OPEN_PARAM
        bcc     :+
        jmp     ERROR

        ; Copy file reference number
:       lda     OPEN_REF
        sta     READ_REF
        sta     CLOSE_REF

        ; Get load address from aux-type
        lda     FILE_INFO_ADDR
        ldx     FILE_INFO_ADDR+1
        sta     READ_ADDR
        stx     READ_ADDR+1

        ; It's high time to leave this place
        jmp     __CODE_0300_RUN__

; ------------------------------------------------------------------------

        .segment        "CODE_0300"

        jsr     MLI
        .byte   READ_CALL
        .word   READ_PARAM
        bcs     ERROR

        jsr     MLI
        .byte   CLOSE_CALL
        .word   CLOSE_PARAM
        bcs     ERROR

        ; Copy REM and startup filename to BASIC input buffer
        ldx     #$00
        lda     #$B2            ; REM token
        bne     :++             ; Branch always
:       inx
        lda     a:STACK-1,x
:       sta     BUF,x
        bne     :--

        ; Go for it ...
        jmp     (READ_ADDR)

PRINT:
        sta     A1L
        stx     A1H
        ldx     VERSION
        ldy     #$00
:       lda     (A1L),y
        beq     :++
        cpx     #$06            ; //e ?
        beq     :+
        cmp     #$60            ; lowercase ?
        bcc     :+
        and     #$5F            ; -> uppercase
:       ora     #$80
        jsr     COUT
        iny
        bne     :--             ; Branch always
:       rts

ERROR:
        cmp     #FILE_NOT_FOUND_ERR
        bne     :+
        lda     #<FILE_NOT_FOUND
        ldx     #>FILE_NOT_FOUND
        jsr     PRINT
        beq     :++             ; Branch always
:       pha
        lda     #<ERROR_NUMBER
        ldx     #>ERROR_NUMBER
        jsr     PRINT
        pla
        jsr     PRBYTE
:       lda     #<PRESS_ANY_KEY
        ldx     #>PRESS_ANY_KEY
        jsr     PRINT
        jsr     RDKEY
        jsr     MLI
        .byte   QUIT_CALL
        .word   QUIT_PARAM
