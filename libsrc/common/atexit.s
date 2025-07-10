;
; Ullrich von Bassewitz, 06.06.1998
;
; int atexit (void (*f) (void));
;

        .export         _atexit
        .destructor     doatexit, 17
        .import         callax

        .include        "errno.inc"

; ---------------------------------------------------------------------------

.proc   _atexit

        ldy     exitfunc_index
        cpy     #exitfunc_max           ; Slot available?
        beq     @Error                  ; Jump if no

; Enter the function into the table

        sta     exitfunc_table,y
        iny
        txa
        sta     exitfunc_table,y
        iny
        sty     exitfunc_index

; Done, return zero

        lda     #0
        tax
        rts

; Error, no space left

@Error: lda     #ENOSPC         ; No space left
        jsr     ___seterrno
        ldx     #$FF            ; Return -1
        txa
Exit:   rts

.endproc



; ---------------------------------------------------------------------------

.code

.proc   doatexit

        ldy     exitfunc_index          ; Get index
        beq     _atexit::Exit           ; Jump if done
        dey
        lda     exitfunc_table,y
        tax
        dey
        lda     exitfunc_table,y
        sty     exitfunc_index
        jsr     callax                  ; Call the function
.if .cap(CPU_HAS_BRA8)
        bra     doatexit
.else
        jmp     doatexit                ; Next one
.endif

.endproc



; ---------------------------------------------------------------------------

.bss
exitfunc_index: .res    1       ; Index into table, inc'ed by 2
exitfunc_table: .res    10      ; 5 exit functions
exitfunc_max    = <(* - exitfunc_table)


