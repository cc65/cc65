;
; Ullrich von Bassewitz, 16.11.2002
;
; File name handling for CBM file I/O
;

        .export         fnparse, fnset, fnaddmode, fncomplete, fndefunit
        .export         fnunit, fnlen, fncmd, fnbuf

        .import         SETNAM
        .import         __curunit, __filetype
        .importzp       ptr1

        .include        "ctype.inc"


;--------------------------------------------------------------------------
; fnparse: Parse a filename passed in in a/x. Will set the following
; variables:
;
;       fnlen   -> length of filename
;       fnbuf   -> filename including drive spec
;       fnunit  -> unit from spec or default unit
;
; Returns an error code in A or zero if all is ok.

.proc   fnparse

        sta     ptr1
        stx     ptr1+1          ; Save pointer to name

; For now we will always use the default unit

        jsr     fndefunit

; Check the name for a drive spec

        ldy     #0
        lda     (ptr1),y
        sta     fnbuf+0
        cmp     #'0'
        beq     digit
        cmp     #'1'
        bne     nodrive

digit:  iny
        lda     (ptr1),y
        cmp     #':'
        bne     nodrive

; We found a drive spec, copy it to the buffer

        sta     fnbuf+1
        iny                     ; Skip colon
        bne     drivedone       ; Branch always

; We did not find a drive spec, always use drive zero

nodrive:
        lda     #'0'
        sta     fnbuf+0
        lda     #':'
        sta     fnbuf+1
        ldy     #$00            ; Reposition to start of name

; Drive spec done. Copy the name into the file name buffer. Check that all
; file name characters are valid and that the maximum length is not exceeded.

drivedone:
        lda     #2              ; Length of drive spec
        sta     fnlen

nameloop:
        lda     (ptr1),y        ; Get next char from filename
        beq     namedone        ; Jump if end of name reached

; Check for valid chars in the file name. We allow letters, digits, plus some
; additional chars from a table.

        ldx     #fncharcount-1
namecheck:
        cmp     fnchars,x
        beq     nameok
        dex
        bpl     namecheck
        tax
        lda     __ctype,x
        and     #CT_ALNUM
        beq     invalidname

; Check the maximum length, store the character

nameok: ldx     fnlen
        cpx     #18             ; Maximum length reached?
        bcs     invalidname
        lda     (ptr1),y        ; Reload char
        jsr     fnadd           ; Add character to name
        iny                     ; Next char from name
        bne     nameloop        ; Branch always

; Invalid file name

invalidname:
        lda     #33             ; Invalid file name

; Done, we've successfully parsed the name.

namedone:
        rts

.endproc

;--------------------------------------------------------------------------
; fndefunit: Use the default unit

.proc   fndefunit

        lda     __curunit
        sta     fnunit
        rts

.endproc

;--------------------------------------------------------------------------
; fnset: Tell the kernal about the file name

.proc   fnset

        lda     fnlen
        ldx     #<fnbuf
        ldy     #>fnbuf
        jmp     SETNAM

.endproc

;--------------------------------------------------------------------------
; fncomplete: Complete a filename by adding ",t,m" where t is the file type
; and m is the access mode passed in in the A register
;
; fnaddmode: Add ",m" to a filename, where "m" is passed in A

fncomplete:
	pha	   		; Save mode
        jsr     fnaddcomma      ; Add a comma
        lda     __filetype
        jsr     fnadd           ; Add the type
        pla
fnaddmode:
        pha
        jsr     fnaddcomma
        pla

fnadd:  ldx     fnlen
        inc     fnlen
        sta     fnbuf,x
        rts

fnaddcomma:
        lda     #','
        bne     fnadd

;--------------------------------------------------------------------------
; Data

.bss

fnunit: .res    1
fnlen:  .res    1

.data
fncmd:  .byte   's'     ; Use as scratch command
fnbuf:  .res    22      ; 0:0123456789012345,t,m

.rodata
; Characters that are ok in filenames besides digits and letters
fnchars:.byte   ".,-_+()"
fncharcount = *-fnchars



