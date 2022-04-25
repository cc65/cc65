;
; jede jede@oric.org 2017-10-01
;
    .export    _cgetc

    .import    cursor

    .export    store_char

    .include   "telestrat.inc"

.proc _cgetc
        ; This routine could be quicker if we wrote in page 2 variables,
        ; But it's better to use telemon routine in that case, because telemon can manage 4 I/O
        ldx     cursor       ; If cursor equal to 0, then switch off cursor
        beq     switchoff_cursor

        ldx     #$00         ; x is the first screen
        BRK_TELEMON(XCSSCR)  ; Display cursor
        jmp     start        ; Could be replaced by a bne/beq but 'jmp' is cleaner than a bne/beq which could expect some matters

switchoff_cursor:
        ; At this step X is equal to $00, X must be set, because it's the id of the screen (telestrat can handle 4 virtuals screen)
        BRK_TELEMON(XCOSCR)  ; Switch off cursor


start:
        lda     store_char   ; Does kbhit store a value in store_char ?
        bne     @out         ; Yes, we returns A and we reset store_char
@wait_key:
        BRK_TELEMON XRD0     ; Waits until key is pressed
        bcs     @wait_key
        ldx     #$00
        rts
@out:
        ldx     #$00
        stx     store_char
        rts
.endproc
.data
store_char:
        .byte 0
