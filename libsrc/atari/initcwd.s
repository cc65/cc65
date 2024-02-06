;
; Stefan Haubenthal, 2008-04-29
;

        .export         initcwd
        .import         findfreeiocb
        .import         __cwd
        .include        "atari.inc"

.proc   initcwd

        lda     #0
        sta     __cwd
        jsr     findfreeiocb
        bne     oserr
        lda     #GETCWD
        sta     ICCOM,x
        lda     #<__cwd
        sta     ICBLL,x
        lda     #>__cwd
        sta     ICBLH,x
        jsr     CIOV
        bmi     oserr
        ldx     #$FF            ; ATEOL -> \0
:       inx
        lda     __cwd,x
        cmp     #ATEOL
        bne     :-
        lda     #0
        sta     __cwd,x
oserr:  rts

.endproc
