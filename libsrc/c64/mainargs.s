; mainargs.s
;
; Ullrich von Bassewitz, 2003-03-07
; Based on code from Stefan A. Haubenthal, <polluks@web.de>
; 2003-05-18, Greg King
;
; Scan a group of arguments that are in BASIC's input-buffer.
; Build an array that points to the beginning of each argument.
; Send, to main(), that array and the count of the arguments.

; Command-lines look like these lines:
;
; run
; run : rem  no arguments because no comma!
; run:rem,arg1," arg 2" , arg 3 ,, arg5, ...
;
; "run" and "rem" are entokenned; the args. are not.  Leading spaces are
; ignored; trailing spaces are included -- unless the argument was quoted.

; TO-DO:
; - The "file-name" might be a path-name; don't copy the directory-components.
; - Add a control-character quoting mechanism.

	.constructor	initmainargs, 24
	.import		__argc, __argv

	.include	"c64.inc"

; Maximum number of arguments allowed in the argument table.
; (An argument contains a comma, at least.)
;
MAXARGS	 = BASIC_BUF_LEN - 2	; (don't count REM and terminating '\0')

REM	 = $8f			; BASIC token-code
NAME_LEN = 16			; maximum length of command-name

; Get possible command-line arguments.
;
initmainargs:

; Assume that the program was loaded, a moment ago, by the traditional LOAD
; statement.  Save the "most-recent filename" as argument #0.
; Because the buffer, that we're copying into, was zeroed out,
; we don't need to add a NUL character.
;
	ldy	FNAM_LEN
	cpy	#NAME_LEN + 1
	bcc	L1
	ldy	#NAME_LEN - 1	; limit the length
L0:	lda	(FNAM),y
	sta	name,y
L1:	dey
	bpl	L0
	lda	#<name
	ldx	#>name
	sta	argv
	stx	argv + 1
	inc	__argc		; argc always is equal to, at least, 1

; Find the "rem" token.
;
	ldx	#0
L2:	lda	BASIC_BUF,x
	beq	done		; no "rem," no args.
	inx
	cmp	#REM
	bne	L2
	ldy	#1 * 2

; Find the next argument.
;
next:	lda	BASIC_BUF,x
	beq	done
	inx
	cmp	#','		; look for argument-list separator
	bne	next
	lda	#$00
	sta	BASIC_BUF-1,x	; make the previous arg. be a legal C string
	inc	__argc		; found another arg.

L4:	lda	BASIC_BUF,x
	beq	point		; zero-length argument
	inx
	cmp	#' '
	beq	L4		; skip leading spaces

	cmp	#'"'		; is argument quoted?
	beq	L5
	dex			; no, don't skip over character
	clc			; (quotation-mark sets flag)
L5:	ror	quoted		; save it

; BASIC's input-buffer starts at the beginning of a RAM page.
; So, we don't need to add the offset -- just store it.
;
point:	txa
	sta	argv,y		; argv[y]= &arg
	iny
	lda	#>BASIC_BUF
	sta	argv,y
	iny

	asl	quoted		; is argument a string-literal?
	bcc	next		; no, don't look for ending quotation-mark
L7:	lda	BASIC_BUF,x
	beq	done
	inx
	cmp	#'"'
	bne	L7
	lda	#$00
	sta	BASIC_BUF-1,x	; make this arg. be a legal C string
	beq	next		;(bra)

; (The last vector in argv[] already is NULL.)
;
done:	lda	#<argv
	ldx	#>argv
	sta	__argv
	stx	__argv + 1
	rts

; These arrays are zeroed before initmainargs is called.
; char	name[16+1];
; char* argv[MAXARGS+1]={name};
;
	.bss
quoted:	.res	1, %00000000
name:	.res	NAME_LEN + 1
argv:	.res	(MAXARGS + 1) * 2
