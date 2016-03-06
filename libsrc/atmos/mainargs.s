;
; 2003-03-07, Ullrich von Bassewitz
; 2011-01-28, Stefan Haubenthal
; 2014-09-10, Greg King
;
; Set up arguments for main
;

        .constructor    initmainargs, 24
        .import         __argc, __argv

        .include        "atmos.inc"
        .macpack        generic

MAXARGS  = 10                   ; Maximum number of arguments allowed
REM      = $9d                  ; BASIC token-code


;---------------------------------------------------------------------------
; Get possible command-line arguments. Goes into the special ONCE segment,
; which may be reused after the startup code is run

.segment        "ONCE"

.proc   initmainargs

; Assume that the program was loaded, a moment ago, by the traditional LOAD
; statement.  Save the "most-recent filename" as argument #0.
; Because the buffer, that we're copying into, was zeroed out,
; we don't need to add a NUL character.
;
        ldy     #FNAME_LEN - 1  ; limit the length
L0:     lda     CFOUND_NAME,y
        sta     name,y
        dey
        bpl     L0
        inc     __argc          ; argc always is equal to, at least, 1

; Find the "rem" token.
;
        ldx     #0
L2:     lda     BASIC_BUF,x
        beq     done            ; no "rem", no args.
        inx
        cmp     #REM
        bne     L2

; The arguments must be copied to a safe place because BASIC's input buffer
; might be re-used by the stdin console.

        ldy     #(SCREEN_XSIZE * 2 - 1) - 1
L3:     lda     BASIC_BUF,y
        sta     args,y
        dey
        bpl     L3

        ldy     #1 * 2          ; Point to second argv slot

; Find the next argument

next:   lda     BASIC_BUF,x
        beq     done            ; End of line reached
        inx
        cmp     #' '            ; Skip leading spaces
        beq     next            ;

; Found start of next argument. We've incremented the pointer in X already, so
; it points to the second character of the argument. This is useful since we
; will check now for a quoted argument, in which case we will have to skip this
; first character.

found:  cmp     #'"'            ; Is the argument quoted?
        beq     setterm         ; Jump if so
        dex                     ; Reset pointer to first argument character
        lda     #' '            ; A space ends the argument
setterm:sta     term            ; Set end of argument marker

; Now, store a pointer, to the argument, into the next slot.

        txa                     ; Get low byte
        add     #<args
        sta     argv,y          ; argv[y]= &arg
        lda     #>$0000
        adc     #>args
        sta     argv+1,y
        iny
        iny
        inc     __argc          ; Found another arg

; Search for the end of the argument

argloop:lda     BASIC_BUF,x
        beq     done
        inx
        cmp     term
        bne     argloop

; We've found the end of the argument. X points one character behind it, and
; A contains the terminating character. To make the argument a valid C string,
; replace the terminating character by a zero.

        lda     #$00
        sta     args-1,x

; Check if the maximum number of command line arguments is reached. If not,
; parse the next one.

        lda     __argc          ; Get low byte of argument count
        cmp     #MAXARGS        ; Maximum number of arguments reached?
        bcc     next            ; Parse next one if not

; (The last vector in argv[] already is NULL.)

done:   lda     #<argv
        ldx     #>argv
        sta     __argv
        stx     __argv + 1
        rts

.endproc

; These arrays are zeroed before initmainargs is called.
; char  name[16+1];
; char* argv[MAXARGS+1]={name};
;
.bss
term:   .res    1
name:   .res    FNAME_LEN + 1
args:   .res    SCREEN_XSIZE * 2 - 1

.data
argv:   .addr   name
        .res    MAXARGS * 2, $00
