;
; Oliver Schmidt, 2011-01-26
; Robert Justice, 2026
;
; int __fastcall__ exec (const char* progname, const char* cmdline);
;

        .export         _exec
        .import         sos_file_info_direct
        .import         pushname_tos, popname, popax, done, _exit

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "apple3.inc"
        .include        "sos.inc"

        ; Wrong file type
typerr: lda     #$4A            ; "Incompatible file format"

        ; Cleanup name

soserr: jsr     popname
oserr:  jmp     ___mappederrno

_exec:
        ; Store cmdline
        sta     ptr4
        stx     ptr4+1

        ; Get and push name
        jsr     pushname_tos
        bne     oserr

        jsr     sos_file_info_direct
        bne     soserr

        ; Patch SOS 1.3 to allow call from high memory
        lda     E_REG
        tax
        and     #$F7             ; remove ram write protect 
        sta     E_REG
        lda     #$B9
        sta     $F294
        sta     $F2b3
        stx     E_REG            ; restore

        ; Copy the path to 64 bytes above interp space
        ldy     #$00
        lda     (c_sp),y
        tay
:       lda     (c_sp),y
        sta     $B8C0,y
        dey
        bpl     :-

        ; If we get here the program file at least exists so we copy
        ; the loader stub right now and patch it later to set params
        ; copy the load stub to just below the path
        ldx     #size - 1
:       lda     source,x
        sta     target,x
        dex
        bpl     :-

        ; Check program file type
        lda     sosoption + OPTION::FILE_INFO::FILE_TYPE
        cmp     #$0C            ; SOS file?
        bne     typerr          ; No, wrong file type

        ; Reset stack as we already passed
        ; the point of no return anyway
        ldx     #$FF
        txs

        ; Call loader stub after C library shutdown
        lda     #<target
        ldx     #>target
        sta     done+1
        stx     done+2

        ; Initiate C library shutdown
        jmp     _exit


        .rodata

source:
        ; Open program file
        ; PATHNAME parameter is already set (we reuse
        ; the copy at $B8C0)
        brk
        .byte   OPEN_CALL
        .word   open_param
        bne     error

        ; Copy REF_NUM to SOS READ and CLOSE parameters
        lda     open_ref
        sta     readh_ref
        sta     read_ref
        sta     close_ref

        ; Read sos.interp header
        brk
        .byte   READ_CALL
        .word   readh_param
        bne     error

        ; Check SOS.NTRP header exists
        ldy     #7
:       lda     interp_h,y
        cmp     header,y
        bne     error
        dey
        bpl     :-

        ; Set load addr (assume no opt_header)
        lda     load_addr
        ldx     load_addr+1
        sta     data_buffer
        stx     data_buffer+1

        ; Read the rest of the program file
        brk
        .byte   READ_CALL
        .word   read_param
        bne     error

        ; Close program file
        brk
        .byte   CLOSE_CALL
        .word   close_param
        bne     error

        ; Go for it ...
jump:   jmp     (data_buffer)


open_param      = * - source + target
        .byte   $04             ; PARAM_COUNT
        .addr   $B8C0           ; PATHNAME
open_ref        = * - source + target  
        .byte   $00             ; refnum
        .word   $0              ; no option list
        .byte   $0

readh_param     = * - source + target  
        .byte   $04             ; PARAM_COUNT
readh_ref       = * - source + target  
        .byte   $00             ; REF_NUM
        .addr   header          ; interp header buffer
        .word   14              ; REQUEST_COUNT
        .word   0               ; TRANS_COUNT

interp_h        = * - source + target 
        .byte   "SOS NTRP"

header          = * - source + target   
        .res    8
        .word   0
load_addr       = * - source + target
        .word   0
        .word   0

read_param      = * - source + target
        .byte   $04             ; PARAM_COUNT
read_ref        = * - source + target
        .byte   $00             ; REF_NUM
data_buffer     = * - source + target
        .addr   $2000           ; DATA_BUFFER
        .word   $FFFF           ; REQUEST_COUNT
        .word   $0000           ; TRANS_COUNT

close_param     = * - source + target
        .byte   $01             ; PARAM_COUNT
close_ref       = * - source + target
        .byte   $00             ; REF_NUM

        ; Quit to SOS
error:  brk
        .byte   $65             ; Terminate
        .word   error

size            = * - source

target          = $B8C0 - size  ; Use $B800-$B8C0
