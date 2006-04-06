; mainargs.s
;
; Ullrich von Bassewitz, 2003-03-07
; Based on code from Stefan A. Haubenthal <polluks@web.de>, 2003-11-08
; Greg King, 2003-05-18
; Stefan Haubenthal, 2005-01-07
; Oliver Schmidt, 2005-04-05
;
; Scan a group of arguments that are in BASIC's input-buffer.
; Build an array that points to the beginning of each argument.
; Send, to main(), that array and the count of the arguments.

; Command-lines look like these lines:
;
; call2048
; call2048 : rem
; call2048:rem arg1 " arg 2 is quoted "  arg3 "" arg5
;
; "call" and "rem" are entokenned; the args. are not.  Leading and trailing
; spaces outside of quotes are ignored.

; TO-DO:
; Add a control-character quoting mechanism.

        .constructor	initmainargs, 18
        .import		__argc, __argv, __dos_type

        .include	"zeropage.inc"
        .include	"apple2.inc"

; Maximum number of arguments allowed in the argument table.
; (An argument contains a comma, at least.)

MAXARGS = 10

BASIC_BUF = $200
FNAM_LEN  = $280
FNAM      = $281
REM       = $B2			; BASIC token-code

; Get possible command-line arguments. Goes into the special INIT segment,
; which may be reused after the startup code is run

        .segment        "INIT"

initmainargs:

; Assume that the program was loaded, a moment ago, by the traditional BLOAD
; statement of BASIC.SYSTEM. Save the "most-recent filename" as argument #0.

        ldx	__dos_type	; No ProDOS -> argv[0] = ""
        beq	:+

; Terminate the filename with a zero to make it a valid C string.

        ldx	FNAM_LEN
:       lda	#$00
        sta	FNAM,x

        inc	__argc		; argc always is equal to, at least, 1

; Find the "rem" token.

        ldx	#$00
:       lda	BASIC_BUF,x
        beq	done		; No "rem" -> no args
        inx
        cmp	#REM
        bne	:-
        ldy	#$01 * 2	; Start with argv[1]

; Find the next argument.

next:   lda	BASIC_BUF,x
        beq	done
        inx
        cmp	#' '		; Skip leading spaces
        beq	next

; Found start of next argument. We've incremented the pointer in X already, so
; it points to the second character of the argument. This is useful since we
; will check now for a quoted argument, in which case we will have to skip this
; first character.

        cmp	#'"'		; Is the argument quoted?
        beq	setterm		; Jump if so
        dex			; Reset pointer to first argument character
        lda	#' '		; A space ends the argument
setterm:sta     tmp1		; Set end of argument marker

; Now store a pointer to the argument into the next slot. Since the BASIC
; input buffer is located at the start of a RAM page, no calculations are
; necessary.

        txa			; Get low byte
        sta	argv,y		; argv[y] = &arg
        iny
        lda	#>BASIC_BUF
        sta	argv,y
        iny
        inc	__argc		; Found another arg

; Search for the end of the argument

argloop:lda     BASIC_BUF,x
        beq	done
        inx
        cmp	tmp1
        bne	argloop

; We've found the end of the argument. X points one character behind it, and
; A contains the terminating character. To make the argument a valid C string,
; replace the terminating character by a zero.

        lda	#$00
        sta	BASIC_BUF-1,x

; Check if the maximum number of command line arguments is reached. If not,
; parse the next one.

        lda	__argc		; Get low byte of argument count
        cmp	#MAXARGS	; Maximum number of arguments reached?
        bcc	next		; Parse next one if not

; (The last vector in argv[] already is NULL.)

done:   lda	#<argv
        ldx	#>argv
        sta	__argv
        stx	__argv+1
        rts

; This array is zeroed before initmainargs is called.
; char* argv[MAXARGS+1] = {FNAM};

        .data

argv:   .addr	FNAM
        .res	MAXARGS * 2
