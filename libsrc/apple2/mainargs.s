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
; call2051
; call2051 : rem
; call2051:rem arg1 " arg 2 is quoted "  arg3 "" arg5
;
; "call" and "rem" are entokenned; the args. are not. Leading and trailing
; spaces outside of quotes are ignored.

; TO-DO:
; Add a control-character quoting mechanism.

        .constructor    initmainargs, 24
        .import         __argc, __argv, __dos_type

        .include        "zeropage.inc"
        .include        "apple2.inc"

; Maximum number of arguments allowed in the argument table.
; (An argument contains a comma, at least.)

MAXARGS = 10

; ProDOS stores the filename in the second half of BASIC's input buffer, so
; there are 128 characters left. At least 1 character is necessary for the
; REM so 127 characters (including the terminating zero) may be used before
; overwriting the ProDOS filename. As we don't want to further restrict the
; command-line length we reserve those 127 characters.

BUF_LEN = 127

BASIC_BUF = $200
FNAM_LEN  = $280
FNAM      = $281
REM       = $B2                 ; BASIC token-code

; Get possible command-line arguments. Goes into the special INIT segment,
; which may be reused after the startup code is run.

        .segment        "INIT"

initmainargs:

; Assume that the program was loaded, a moment ago, by the traditional BLOAD
; statement of BASIC.SYSTEM. Save the filename as argument #0 if available.

        ldx     __dos_type      ; No ProDOS -> argv[0] = ""
        beq     :+

; Terminate the filename with a zero to make it a valid C string.

        ldx     FNAM_LEN
:       lda     #$00
        sta     FNAM,x

        inc     __argc          ; argc always is equal to, at least, 1

; Find the "rem" token.

        ldx     #$00
:       lda     BASIC_BUF,x
        beq     done            ; No "rem" -> no args
        inx
        cmp     #REM
        bne     :-

; If a clock is present it is called by ProDOS on file operations. On machines
; with a slot-based clock (like the Thunder Clock) the clock firmware places
; the current date in BASIC's input buffer. Therefore we have to create a copy
; of the command-line in a different buffer before the original is potentially
; destroyed.

        ldy     #$00
:       lda     BASIC_BUF,x
        sta     buffer,y
        inx
        iny
        cpy     #BUF_LEN - 1    ; Keep the terminating zero intact
        bcc     :-

; Start processing the arguments.

        ldx     #$00
        ldy     #$01 * 2        ; Start with argv[1]

; Find the next argument. Stop if the end of the string or a character with the
; hibit set is reached. The later is true if the string isn't already parsed by
; BASIC (as expected) but is a still unprocessed input string. In this case the
; string isn't the expected command-line at all. We found this out the hard way
; by BRUNing the program with ProDOS on a machine with a slot-based clock (like
; the Thunder Clock). ProDOS called the clock firmware which places the current
; date as BASIC input string with hibits set in the input buffer. While looking
; for the REM token we stumbled across the first '2' character ($32+$80 = $B2)
; and interpreted the rest of the date as a spurious command-line parameter.

next:   lda     buffer,x
        beq     done
        bmi     done
        inx
        cmp     #' '            ; Skip leading spaces
        beq     next

; Found start of next argument. We've incremented the pointer in X already, so
; it points to the second character of the argument. This is useful since we
; will check now for a quoted argument, in which case we will have to skip this
; first character.

        cmp     #'"'            ; Is the argument quoted?
        beq     :+              ; Jump if so
        dex                     ; Reset pointer to first argument character
        lda     #' '            ; A space ends the argument
:       sta     tmp1            ; Set end of argument marker

; Now store a pointer to the argument into the next slot.

        txa                     ; Get low byte
        clc
        adc     #<buffer
        sta     argv,y          ; argv[y] = &arg
        iny
        lda     #$00
        adc     #>buffer
        sta     argv,y
        iny
        inc     __argc          ; Found another arg

; Search for the end of the argument.

:       lda     buffer,x
        beq     done
        inx
        cmp     tmp1
        bne     :-

; We've found the end of the argument. X points one character behind it, and
; A contains the terminating character. To make the argument a valid C string,
; replace the terminating character by a zero.

        lda     #$00
        sta     buffer-1,x

; Check if the maximum number of command-line arguments is reached. If not,
; parse the next one.

        lda     __argc          ; Get low byte of argument count
        cmp     #MAXARGS        ; Maximum number of arguments reached?
        bcc     next            ; Parse next one if not

; (The last vector in argv[] already is NULL.)

done:   lda     #<argv
        ldx     #>argv
        sta     __argv
        stx     __argv+1
        rts

; This array is zeroed before initmainargs is called.
; char* argv[MAXARGS+1] = {FNAM};

        .data

argv:   .addr   FNAM
        .res    MAXARGS * 2

        .bss

buffer: .res    BUF_LEN
