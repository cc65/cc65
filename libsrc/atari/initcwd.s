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
        ldx     #0              ; ATEOL -> \0
:       lda     __cwd,x
        inx
        cmp     #ATEOL
        bne     :-
        lda     #0
        sta     __cwd-1,x
oserr:  rts

.endproc
