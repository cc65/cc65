;
; Ullrich von Bassewitz, 2003-03-14
;
; int __fastcall__ raise (int sig);
;

        .import         jmpvec

        .include        "signal.inc"


;----------------------------------------------------------------------------
; int __fastcall__ raise (int sig);


_raise:
        cpx     #0
        bne     invalidsig
        cmp     #SIGCOUNT
        bcs     invalidsig

; Save the signal number low byte, then setup the function vector

        pha
        asl     a
        tax
        lda     sigtable,x
        sta     jmpvec+1
        lda     sigtable+1,x
        sta     jmpvec+2
                            
; Reset the signal handler to SIG_DFL (I don't like this because it may
; introduce race conditions, but it's the simplest way to satisfy the 
; standard).

        lda     #<__sig_dfl
        sta     sigtable,x
        lda     #>__sig_dfl
        sta     sigtable+1,x

; Restore the signal number and call the function

        pla                     ; Low byte
        ldx     #0              ; High byte
        jsr     jmpvec          ; Call signal function

; raise() returns zero on success and any other value on failure

        lda     #0
        tax
invalidsig:
        rts

            
