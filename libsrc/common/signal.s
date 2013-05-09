;
; Ullrich von Bassewitz, 2002-12-16
;
; __sigfunc __fastcall__ signal (int sig, __sigfunc func);
;

        .import         popax
        .importzp       ptr1

        .include        "signal.inc"
        .include        "errno.inc"


; Default signal functions: The standard specifies explicitly that the values
; for SIG_IGN and SIG_DFL must be distinct, so we make them so by using both
; rts exits we have. This works because signal functions are __fastcall__, so
; we don't have arguments on the stack.


;----------------------------------------------------------------------------
; __sigfunc __fastcall__ signal (int sig, __sigfunc func);


_signal:
        sta     ptr1
        stx     ptr1+1          ; Remember func

        jsr     popax           ; Get sig

        cpx     #0
        bne     invalidsig
        cmp     #SIGCOUNT
        bcs     invalidsig

; Signal number is valid. Replace the pointer in the table saving the old
; value temporarily on the stack.

        asl     a               ; Prepare for word access
        tax

        sei                     ; Disable interrupts in case of async signals
        lda     sigtable,x
        pha
        lda     ptr1
        sta     sigtable,x
        lda     sigtable+1,x
        pha
        lda     ptr1+1
        sta     sigtable+1,x
        cli                     ; Reenable interrupts

; Get the old value from the stack and return it

        pla
        tax
        pla
__sig_ign:
        rts

; Error entry: We use our knowledge that SIG_ERR is zero here to save a byte

invalidsig:
        lda     #<EINVAL
        jsr     __seterrno      ; Returns 0 in A
        tax                     ; A/X = 0
__sig_dfl:
        rts

