;
; Oliver Schmidt, 2011-01-26
;
; int __fastcall__ exec (const char* progname, const char* cmdline);
;

        .export         _exec
        .import         mli_file_info_direct
        .import         pushname, popname, popax, done, _exit

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "apple2.inc"
        .include        "mli.inc"

        ; Wrong file type
typerr: lda     #$4A            ; "Incompatible file format"

        ; Cleanup name

mlierr: jsr     popname
oserr:  jmp     ___mappederrno

_exec:
        ; Store cmdline
        sta     ptr4
        stx     ptr4+1

        ; Get and push name
        jsr     popax
        jsr     pushname
        bne     oserr

        jsr     mli_file_info_direct
        bcs     mlierr

        ; ProDOS TechRefMan, chapter 5.1.5.1:
        ; "The complete or partial pathname of the system program
        ;  is stored at $280, starting with a length byte."
        ; In fact BASIC.SYSTEM does the same for BLOAD and BRUN of
        ; binary programs so we should do the same too in any case
        ; especially as _we_ rely on it in mainargs.s for argv[0]
        ldy     #$00
        lda     (sp),y
        tay
:       lda     (sp),y
        sta     $0280,y
        dey
        bpl     :-

        ; If we get here the program file at least exists so we copy
        ; the loader stub right now and patch it later to set params
        ldx     #size - 1
:       lda     source,x
        sta     target,x
        dex
        bpl     :-

        ; Check program file type
        lda     mliparam + MLI::INFO::FILE_TYPE
        cmp     #$FF            ; SYS file?
        bne     binary          ; No, check for BIN file
        sta     file_type       ; Save file type for cmdline handling

        ; SYS programs replace BASIC.SYSTEM so set in the ProDOS system bit map
        ; protection for pages $80 - $BF just in case BASIC.SYSTEM is there now
        ldx     #$0F            ; Start with protection for pages $B8 - $BF
        lda     #%00000001      ; Protect only system global page
:       sta     $BF60,x         ; Set protection for 8 pages
        lda     #%00000000      ; Protect no page
        dex
        bpl     :-
        bmi     prodos          ; Branch always

binary: cmp     #$06            ; BIN file?
        bne     typerr          ; No, wrong file type

        ; Set BIN program load addr
        lda     mliparam + MLI::INFO::AUX_TYPE
        ldx     mliparam + MLI::INFO::AUX_TYPE+1
        sta     data_buffer
        stx     data_buffer+1

        ; Check ProDOS system bit map for presence of BASIC.SYSTEM
        lda     $BF6F           ; Protection for pages $B8 - $BF
        cmp     #%00000001      ; Exactly system global page is protected
        beq     setvec

        ; Get highest available mem addr from BASIC.SYSTEM
        ldx     HIMEM+1         ; High byte
        bne     setbuf          ; Branch always

        ; BIN programs are supposed to quit through one of the two DOS
        ; vectors so we set up those to point to the ProDOS dispatcher
setvec: ldx     #$03 - 1        ; Size of JMP opcode
:       lda     dosvec,x
        sta     DOSWARM,x       ; DOS warm start
        sta     DOSWARM + 3,x   ; DOS cold start
        dex
        bpl     :-

        ; No BASIC.SYSTEM so use addr of ProDOS system global page
prodos: ldx     #>$BF00         ; High byte

        ; The I/O buffer needs to be page aligned
setbuf: lda     #$00            ; Low byte

        ; The I/O buffer needs four pages
        dex
        dex
        dex
        dex

        ; Set OPEN MLI call I/O buffer parameter
        sta     io_buffer
        stx     io_buffer+1

        .ifdef  __APPLE2ENH__
        ; Calling the 80 column firmware needs the ROM switched
        ; in, otherwise it copies the F8 ROM to the LC (@ $CEF4)
        bit     $C082

        ; ProDOS TechRefMan, chapter 5.3.1.3:
        ; "80-column text cards -- and other Apple IIe features -- can
        ;  be turned off using the following sequence of instructions:"
        lda     #$15
        jsr     $C300

        ; Switch in LC bank 2 for R/O
        bit     $C080
        .endif

        ; Reset stack as we already passed
        ; the point of no return anyway
        ldx     #$FF
        txs

        ; Store up to 127 chars of cmdline (if any)
        ; including terminating zero in stack page
        ldy     #$00
        lda     ptr4+1          ; NULL?
        beq     :++             ; Yes, store as '\0'
:       lda     (ptr4),y
:       sta     $0100,y
        beq     :+              ; '\0' stored, done
        iny
        cpy     #$7E
        bcc     :--
        lda     #$00            ; '\0'
        beq     :-              ; Branch always

        ; Call loader stub after C library shutdown
:       lda     #<target
        ldx     #>target
        sta     done+1
        stx     done+2

        ; Initiate C library shutdown
        jmp     _exit

        .rodata

source:
        ; Open program file
        ; PATHNAME parameter is already set (we reuse
        ; the copy at $0280); IO_BUFFER has been setup
        ; before shutting down the C library
        jsr     $BF00
        .byte   OPEN_CALL
        .word   open_param
        bcs     error

        ; Copy REF_NUM to MLI READ and CLOSE parameters
        lda     open_ref
        sta     read_ref
        sta     close_ref

        ; Read whole program file
        jsr     $BF00
        .byte   READ_CALL
        .word   read_param
        bcs     error

        ; Close program file
        jsr     $BF00
        .byte   CLOSE_CALL
        .word   close_param
        bcs     error

        ; Check for cmdline handling
        ldx     file_type       ; SYS file?
        bne     system          ; Yes, check for startup filename

        ; Store REM and cmdline in BASIC input buffer
        lda     #$B2            ; REM token
        bne     :++             ; Branch always
:       inx
        lda     a:$0100-1,x
:       sta     $0200,x
        bne     :--
        beq     jump            ; Branch always

        ; Check for startup filename support
        ; ProDOS TechRefMan, chapter 5.1.5.1:
        ; "$2000 is a jump instruction. $2003 and $2004 are $EE."
system: lda     #$4C
        cmp     $2000
        bne     jump
        lda     #$EE
        cmp     $2003
        bne     jump
        cmp     $2004
        bne     jump

        ; Store cmdline in startup filename buffer
        ldx     #$01
:       lda     a:$0100-1,x
        beq     :+
        sta     $2006,x
        inx
        cpx     $2005           ; Buffer full?
        bcc     :-              ; No, continue
:       dex
        stx     $2006           ; Store cmdline length

        ; Go for it ...
jump:   jmp     (data_buffer)

file_type       = * - source + target
        .byte   $00

open_param      = * - source + target
        .byte   $03             ; PARAM_COUNT
        .addr   $0280           ; PATHNAME
io_buffer       = * - source + target
        .addr   $0000           ; IO_BUFFER
open_ref        = * - source + target
        .byte   $00             ; REF_NUM

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

        ; Quit to ProDOS dispatcher
quit            = * - source + target
error:  jsr     $BF00
        .byte   $65             ; QUIT
        .word   quit_param

quit_param      = * - source + target
        .byte   $04             ; PARAM_COUNT
        .byte   $00             ; QUIT_TYPE
        .word   $0000           ; RESERVED
        .byte   $00             ; RESERVED
        .word   $0000           ; RESERVED

size            = * - source

target          = DOSWARM - size

        ; Make sure that the loader isn't too big, and
        ; fits in $300-$3D0
        .assert target >= $300, error

dosvec: jmp     quit
