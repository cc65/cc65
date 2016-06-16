;
; Christian Groessler, 12-Jun-2016
;
; int __fastcall__ exec (const char* progname, const char* cmdline);
;
; supports only XDOS at the moment

        .export         _exec

        .import         popax
        .import         __dos_type
        .import         findfreeiocb
        .import         incsp2
        .import         excexit                 ; from crt0.s
        .import         SP_save                 ; from crt0.s
.ifdef  UCASE_FILENAME
        .import         ucase_fn
        .import         addysp
.endif

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "atari.inc"

; area $0100 to $0128 might be in use (e.g. Hias' high speed patch)
CMDLINE_BUFFER          =       $0129           ; put progname + cmdline as one single string there
; alternatively:
;CMDLINE_BUFFER          =       $0480           ; put progname + cmdline as one single string there
CMDLINE_MAX             =       40+3            ; max. length of drive + progname + cmdline

        .code

notsupp:lda     #ENOSYS         ; "unsupported system call"
        .byte   $2C             ; bit opcode, eats the next 2 bytes
noiocb: lda     #EMFILE         ; "too many open files"
        jsr     incsp2          ; clean up stack
seterr: jmp     __directerrno


; entry point

_exec:
        ; save cmdline
        sta     ptr3
        stx     ptr3+1

        ldy     __dos_type
        cpy     #XDOS
        bne     notsupp

        jsr     findfreeiocb
        bne     noiocb

        stx     tmp4            ; remember IOCB index

        ; get program name
        jsr     popax

.ifdef  UCASE_FILENAME
.ifdef  DEFAULT_DEVICE
        ldy     #$80
.else
        ldy     #$00
.endif
        sty     tmp2            ; set flag for ucase_fn
        jsr     ucase_fn
        bcc     ucok1
invret: lda     #EINVAL         ; file name is too long
        bne     seterr
ucok1:
.endif  ; defined UCASE_FILENAME

; copy program name and arguments to CMDLINE_BUFFER

        sta     ptr4            ; ptr4: pointer to program name
        stx     ptr4+1
        ldy     #0
        ; TODO: check stack ptr and and use min(CMDLINE_MAX,available_stack)
copyp:  lda     (ptr4),y
        beq     copypd
        sta     CMDLINE_BUFFER,y
        iny
        cpy     #CMDLINE_MAX
        bne     copyp

        ; programe name too long
        beq     invret

.ifndef  UCASE_FILENAME
invret: lda     #EINVAL
        bne     seterr
.endif

; file name copied, check for args

copypd: tya                     ; put Y into X (index into CMDLINE_BUFFER)
        tax
        lda     ptr3
        ora     ptr3+1          ; do we have arguments?
        beq     copycd          ; no
        ldy     #0
        lda     (ptr3),y        ; get first byte of cmdline parameter
        beq     copycd          ; nothing there...
        lda     #' '            ; add a space btw. progname and cmdline
        bne     copyc1

; copy args

copyc:  lda     (ptr3),y
        beq     copycd
        iny
copyc1: sta     CMDLINE_BUFFER,x
        inx
        cpx     #CMDLINE_MAX
        bne     copyc
        ; progname + arguments too long
        beq     invret

invexe: jsr     close
        lda     #XNTBIN
        bne     setmerr

copycd: lda     #ATEOL
        sta     CMDLINE_BUFFER,x

; open the program file, read the first two bytes and compare them to $FF

        ldx     tmp4            ; get IOCB index
        lda     ptr4            ; ptr4 points to progname
        sta     ICBAL,x
        lda     ptr4+1
        sta     ICBAH,x
        lda     #OPNIN          ; open for input
        sta     ICAX1,x
        lda     #OPEN
        sta     ICCOM,x
        jsr     CIOV

        tya

.ifdef  UCASE_FILENAME
        ldy     tmp3            ; get size
        jsr     addysp          ; free used space on the stack
        ; the following 'bpl' depends on 'addysp' restoring A as last command before 'rts'
.endif  ; defined UCASE_FILENAME

        bpl     openok
        pha                     ; remember error code
        jsr     close           ; close the IOCB (required even if open failed)
        pla                     ; put error code back into A
setmerr:jmp     __mappederrno   ; update errno from OS specific error code in A

openok: lda     #>buf
        sta     ICBAH,x         ; set buffer address
        lda     #<buf
        sta     ICBAL,x
        lda     #0              ; set buffer length
        sta     ICBLH,x
        lda     #2
        sta     ICBLL,x
        lda     #GETCHR         ; iocb command code
        sta     ICCOM,x
        jsr     CIOV            ; read it
        bmi     invexe          ; read operation failed, return error

        lda     ICBLL,x         ; # of bytes read
        cmp     #2
        bne     invexe
        lda     #$FF            ; check file format (need $FFFF at the beginning)
        cmp     buf
        bne     invexe
        cmp     buf+1
        bne     invexe

        jsr     close           ; close program file

; program file appears to be available and good
; here's the point of no return

        ldx     SP_save
        txs                     ; reset stack pointer to what it was at program entry
        lda     tmp4            ; get IOCB index
        pha                     ; and save it ('excexit' calls destructors and they might destroy tmp4)
        jsr     excexit         ; on atarixl this will enable the ROM again, making all high variables inaccessible
        pla
        tax                     ; IOCB index in X

        lda     #<CMDLINE_BUFFER
        sta     ICBAL,x         ; address
        lda     #>CMDLINE_BUFFER
        sta     ICBAH,x
        lda     #0
        sta     ICBLL,x         ; length shouldn't be random, but 0 is ok
        sta     ICBLH,x
        sta     ICAX1,x
        sta     ICAX2,x
        lda     #80             ; XDOS: run DUP command
        sta     ICCOM,x
        jmp     CIOV_org        ; no way to display an error message in case of failure, and we will return to DOS


; close IOCB, index in X
.proc   close
        lda    #CLOSE
        sta    ICCOM,x
        jmp    CIOV             ; close IOCB
.endproc

        .bss

buf:    .res    2
