;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                               ;
; LOADER.SYSTEM - an Apple][ ProDOS 8 loader for cc65 programs (Oliver Schmidt) ;
; Use ENABLE_DECOMPRESSOR for ZX-compressed binary handling (Colin Leroy-Mira)  ;
; Use ZX02_DECOMPRESSOR_SHOW_PROGRESS for a decompression progress indicator.   ;
;                                                                               ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CH              := $24
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

.ifndef ENABLE_DECOMPRESSOR
; We want the ProDOS IO buffer as high as possible, as we'll read from start
; address and up.
PRODOS_BUF      := MLI - 1024
.else
; We want to ProDOS IO buffer as low as possible, so we can put the compressed
; data as high as possible in order to decompress in-place without overwriting.
PRODOS_BUF      := $800
.endif

QUIT_CALL          = $65
GET_FILE_INFO_CALL = $C4
OPEN_CALL          = $C8
READ_CALL          = $CA
CLOSE_CALL         = $CC
FILE_NOT_FOUND_ERR = $46

.ifdef ENABLE_DECOMPRESSOR
; Decompressor variables
offset_hi = $80
bitr      = $81
ZX0_src   = $82
ZX0_dst   = $84
pntr      = $86
.endif
; ------------------------------------------------------------------------

        .import __CODE_0280_SIZE__, __DATA_0280_SIZE__
        .import __CODE_0280_LOAD__, __CODE_0280_RUN__

; ------------------------------------------------------------------------

        .segment        "DATA_2000"

GET_FILE_INFO_PARAM:
                .byte   $0A             ;PARAM_COUNT
                .addr   PATHNAME        ;PATHNAME
                .byte   $00             ;ACCESS
                .byte   $00             ;FILE_TYPE
FILE_INFO_ADDR: .word   $0000           ;AUX_TYPE
                .byte   $00             ;STORAGE_TYPE
FILE_BLOCKS:    .word   $0000           ;BLOCKS_USED
                .word   $0000           ;MOD_DATE
                .word   $0000           ;MOD_TIME
                .word   $0000           ;CREATE_DATE
                .word   $0000           ;CREATE_TIME

OPEN_PARAM:
                .byte   $03             ;PARAM_COUNT
                .addr   PATHNAME        ;PATHNAME
                .addr   PRODOS_BUF      ;IO_BUFFER
OPEN_REF:       .byte   $00             ;REF_NUM

LOADING:
                .byte   $0D
                .asciiz "Loading "

ELLIPSES:
                .byte   " ...", $00

; ------------------------------------------------------------------------

        .segment        "DATA_0280"

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

.ifdef ENABLE_DECOMPRESSOR
FINAL_START_ADDR:
                .addr   $0000
.endif

FILE_NOT_FOUND:
                .asciiz "... File not found"

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
PARAMS: .res    $7F

        ; Reset stack
:       ldx     #$FF
        txs

        ; Remove ".SYSTEM" from pathname - this is our default bin file to exec.
        lda     PATHNAME
        sec
        sbc     #.strlen(".SYSTEM")
        sta     PATHNAME

        ; Add trailing '\0' to pathname
        tax
        lda     #$00
        sta     PATHNAME+1,x

        ; Handle command-line:
        ; - no parameters at all: go exec bin file
        ; - any parameters, not starting with '-': copy to STACK,
        ;   where cc65 lib will look for parameters
        ; - any parameters, starting with '-': use the first parameter as binary
        ;   file to load, and the rest as actual parameters

        ldx     PARAMS
        beq     load_file

        ldx     #$00            ; Does the first arg start with -?
        lda     PARAMS+1
        cmp     #'-'
        bne     copy_parameters

copy_pathname:
        lda     PARAMS+2,x      ; Yes, so start copying it (minus the dash) to PATHNAME
        beq     execname_copied ; We're done on NULL or space.
        cmp     #' '
        beq     execname_copied
        sta     PATHNAME+1,x
        inx
        bne     copy_pathname

execname_copied:                ; Terminate PATNAME again and store its length
        lda     #$00
        sta     PATHNAME+1,x
        stx     PATHNAME

        inx                     ; Increment to compensate for the dash,
        inx                     ; increment to avoid doubling the argument separator
copy_parameters:
        ldy     #$00            ; And copy the rest to STACK.
:       lda     PARAMS+1,x
        sta     STACK,y
        beq     load_file
        inx
        iny
        bne     :-

load_file:
        ; Provide some user feedback
        lda     #<LOADING
        ldx     #>LOADING
        jsr     PRINT_2000
        lda     #<(PATHNAME+1)
        ldx     #>(PATHNAME+1)
        jsr     PRINT_2000
        lda     #<ELLIPSES
        ldx     #>ELLIPSES
        jsr     PRINT_2000

        jsr     MLI
        .byte   GET_FILE_INFO_CALL
        .word   GET_FILE_INFO_PARAM
        bcc     :+
        jmp     ERROR_2000

:       jsr     MLI
        .byte   OPEN_CALL
        .word   OPEN_PARAM
        bcc     file_opened
        jmp     ERROR_2000

file_opened:
        ; Now we don't need PATHNAME anymore, and can relocate CODE_0280 and DATA_0280.
.ifndef ENABLE_DECOMPRESSOR
        .assert (__CODE_0280_SIZE__ + __DATA_0280_SIZE__) < $100, error
.endif
        ldx     #$00
:       lda     __CODE_0280_LOAD__,x
        sta     __CODE_0280_RUN__,x
        dex
        bne     :-

.ifdef ENABLE_DECOMPRESSOR
        ; We expect to copy more than one full page and less than three.
        .assert (>(__CODE_0280_SIZE__ + __DATA_0280_SIZE__)) = 1, error
        ; and second page
        ldx     #<(__CODE_0280_SIZE__ + __DATA_0280_SIZE__)
        beq     relocate_done
:       lda     __CODE_0280_LOAD__+256-1,x
        sta     __CODE_0280_RUN__+256-1,x
        dex
        bne     :-
.endif

relocate_done:
        ; Copy file reference number
        lda     OPEN_REF
        sta     READ_REF
        sta     CLOSE_REF

.ifdef ENABLE_DECOMPRESSOR
        ; Compute where to place compressed data, at top of memory, so we can
        ; uncompress in-place without overwriting the compressed data.

        ; Blocks to bytes - caveat: limited to $7F blocks, which will be enough
        ; anyway (65024 bytes...).
        asl     FILE_BLOCKS   ; Blocks are 512 bytes

        lda     #<MLI  ; Make sure we don't touch ProDOS's zone
        sta     READ_ADDR
        sta     ZX0_src

        sec
        lda     #>MLI
        sbc     FILE_BLOCKS
        sta     READ_ADDR+1
        sta     ZX0_src+1
.else
        ; Read directly to the program's start address.
        lda     FILE_INFO_ADDR
        ldx     FILE_INFO_ADDR+1
        sta     READ_ADDR
        stx     READ_ADDR+1
.endif
        ; It's high time to leave this place
        jmp     __CODE_0280_RUN__

; ------------------------------------------------------------------------

        .segment        "CODE_0280"

        ; Read data
        jsr     MLI
        .byte   READ_CALL
        .word   READ_PARAM
        bcs     ERROR

        ; Close the file
        jsr     MLI
        .byte   CLOSE_CALL
        .word   CLOSE_PARAM
        bcs     ERROR

.ifdef ENABLE_DECOMPRESSOR
        ; Get program start address from aux-type. That's where we'll
        ; uncompress.
        lda     FILE_INFO_ADDR
        ldx     FILE_INFO_ADDR+1
        ; Store it as destination for zx decompression,
        sta     ZX0_dst
        stx     ZX0_dst+1
        ; And remember it for the final jump, as both ZX0_dst and
        ; FILE_INFO_ADDR will/might be overwritten during decompression.
        sta     FINAL_START_ADDR
        stx     FINAL_START_ADDR+1
.endif

        ; Copy REM and startup filename to BASIC input buffer
        ldx     #$00
        lda     #$B2            ; REM token
        bne     :++             ; Branch always
:       inx
        lda     a:STACK-1,x
:       sta     BUF,x
        bne     :--

.ifdef ENABLE_DECOMPRESSOR
        ; We've loaded our compressed program, decompress it now.
        jsr     _decompress_zx02_direct
.endif

        ; Clear two lines
        lda     #($0D|$80)
        jsr     COUT
        jsr     COUT

        ; And go for it!
.ifdef ENABLE_DECOMPRESSOR
        jmp     (FINAL_START_ADDR)
.else
        jmp     (READ_ADDR)
.endif

; Define PRINT_2000 entrypoint for use before relocation
PRINT_2000 = * - __CODE_0280_RUN__ + __CODE_0280_LOAD__
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

; Define ERROR_2000 entrypoint for use before relocation
ERROR_2000 = * - __CODE_0280_RUN__ + __CODE_0280_LOAD__
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

.ifdef ENABLE_DECOMPRESSOR
        .include "../../common/zx02_direct.inc"
.endif
